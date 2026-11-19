#include "WasmExecution.hpp"

#include "semantics/symbols/Symbol.hpp"
#include "semantics/types/Type.hpp"

#include <atomic>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <cerrno>
#include <fcntl.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
extern char **environ;
#endif

#ifndef IMP_WASM_RUNNER_PATH
#define IMP_WASM_RUNNER_PATH "runtime/runner.mjs"
#endif

namespace {

struct ProcessResult {
  int status = -1;
  std::string stdout_text;
  std::string stderr_text;
};

std::string read_file(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(file),
          std::istreambuf_iterator<char>()};
}

std::string path_to_utf8(const std::filesystem::path &path);

#ifdef _WIN32
class WinHandle {
public:
  explicit WinHandle(HANDLE value = nullptr) : value_(value) {}
  ~WinHandle() {
    if (value_ && value_ != INVALID_HANDLE_VALUE)
      CloseHandle(value_);
  }
  WinHandle(const WinHandle &) = delete;
  WinHandle &operator=(const WinHandle &) = delete;
  HANDLE get() const { return value_; }

private:
  HANDLE value_;
};

std::wstring to_wide(std::string_view text) {
  if (text.empty())
    return {};
  const int size =
      MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                          static_cast<int>(text.size()), nullptr, 0);
  if (size == 0)
    throw std::runtime_error("invalid UTF-8 in process argument");
  std::wstring result(static_cast<size_t>(size), L'\0');
  if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(),
                           static_cast<int>(text.size()), result.data(),
                           size)) {
    throw std::runtime_error("cannot convert process argument to UTF-16");
  }
  return result;
}

std::wstring path_to_wide(const std::filesystem::path &path) {
  return to_wide(path_to_utf8(path));
}

std::wstring quote_windows_argument(std::wstring_view argument) {
  std::wstring quoted = L"\"";
  size_t backslashes = 0;
  for (wchar_t c : argument) {
    if (c == L'\\') {
      ++backslashes;
    } else if (c == L'"') {
      quoted.append(backslashes * 2 + 1, L'\\');
      quoted += c;
      backslashes = 0;
    } else {
      quoted.append(backslashes, L'\\');
      backslashes = 0;
      quoted += c;
    }
  }
  quoted.append(backslashes * 2, L'\\');
  quoted += L'"';
  return quoted;
}

HANDLE open_child_output(const std::filesystem::path &path) {
  SECURITY_ATTRIBUTES security{sizeof(security), nullptr, TRUE};
  HANDLE handle =
      CreateFileW(path_to_wide(path).c_str(), GENERIC_WRITE,
                  FILE_SHARE_READ | FILE_SHARE_WRITE, &security, CREATE_ALWAYS,
                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
  if (handle == INVALID_HANDLE_VALUE) {
    throw std::runtime_error("cannot create child output file (Windows error " +
                             std::to_string(GetLastError()) + ")");
  }
  return handle;
}

ProcessResult run_process(const std::vector<std::string> &arguments,
                          const std::filesystem::path &stdout_path,
                          const std::filesystem::path &stderr_path) {
  WinHandle stdout_handle(open_child_output(stdout_path));
  WinHandle stderr_handle(open_child_output(stderr_path));
  SECURITY_ATTRIBUTES input_security{sizeof(input_security), nullptr, TRUE};
  WinHandle stdin_handle(CreateFileW(
      L"NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &input_security,
      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
  if (stdin_handle.get() == INVALID_HANDLE_VALUE)
    throw std::runtime_error("cannot open null input handle");

  std::wstring command_line;
  for (const auto &argument : arguments) {
    if (!command_line.empty())
      command_line += L' ';
    command_line += quote_windows_argument(to_wide(argument));
  }

  SIZE_T attribute_bytes = 0;
  InitializeProcThreadAttributeList(nullptr, 1, 0, &attribute_bytes);
  std::vector<std::byte> attribute_storage(attribute_bytes);
  auto *attributes =
      reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attribute_storage.data());
  if (!InitializeProcThreadAttributeList(attributes, 1, 0, &attribute_bytes)) {
    throw std::runtime_error("cannot initialize Windows process attributes");
  }
  struct AttributeListCleanup {
    LPPROC_THREAD_ATTRIBUTE_LIST value;
    ~AttributeListCleanup() { DeleteProcThreadAttributeList(value); }
  } cleanup{attributes};

  HANDLE inherited_handles[] = {stdin_handle.get(), stdout_handle.get(),
                                stderr_handle.get()};
  if (!UpdateProcThreadAttribute(
          attributes, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, inherited_handles,
          sizeof(inherited_handles), nullptr, nullptr)) {
    throw std::runtime_error("cannot configure inherited Windows handles");
  }

  STARTUPINFOEXW startup{};
  startup.StartupInfo.cb = sizeof(startup);
  startup.StartupInfo.dwFlags = STARTF_USESTDHANDLES;
  startup.StartupInfo.hStdOutput = stdout_handle.get();
  startup.StartupInfo.hStdError = stderr_handle.get();
  startup.StartupInfo.hStdInput = stdin_handle.get();
  startup.lpAttributeList = attributes;

  PROCESS_INFORMATION process{};
  if (!CreateProcessW(nullptr, command_line.data(), nullptr, nullptr, TRUE,
                      EXTENDED_STARTUPINFO_PRESENT | CREATE_NO_WINDOW, nullptr,
                      nullptr, &startup.StartupInfo, &process)) {
    const DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) {
      return {127, {}, {}};
    }
    throw std::runtime_error("cannot start child process (Windows error " +
                             std::to_string(error) + ")");
  }

  WinHandle process_handle(process.hProcess);
  WinHandle thread_handle(process.hThread);
  if (WaitForSingleObject(process_handle.get(), INFINITE) != WAIT_OBJECT_0) {
    throw std::runtime_error("cannot wait for child process");
  }
  DWORD exit_code = 1;
  if (!GetExitCodeProcess(process_handle.get(), &exit_code)) {
    throw std::runtime_error("cannot read child process exit code");
  }
  return {static_cast<int>(exit_code), read_file(stdout_path),
          read_file(stderr_path)};
}
#else
ProcessResult run_process(const std::vector<std::string> &arguments,
                          const std::filesystem::path &stdout_path,
                          const std::filesystem::path &stderr_path) {
  std::vector<char *> argv;
  argv.reserve(arguments.size() + 1);
  for (const auto &argument : arguments)
    argv.push_back(const_cast<char *>(argument.c_str()));
  argv.push_back(nullptr);

  posix_spawn_file_actions_t actions;
  int error = posix_spawn_file_actions_init(&actions);
  if (error != 0)
    throw std::runtime_error("cannot initialize child process actions: " +
                             std::string(std::strerror(error)));
  struct ActionsCleanup {
    posix_spawn_file_actions_t *value;
    ~ActionsCleanup() { posix_spawn_file_actions_destroy(value); }
  } cleanup{&actions};

  error = posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO,
                                           stdout_path.c_str(),
                                           O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (error == 0) {
    error = posix_spawn_file_actions_addopen(
        &actions, STDERR_FILENO, stderr_path.c_str(),
        O_WRONLY | O_CREAT | O_TRUNC, 0600);
  }
  if (error != 0)
    throw std::runtime_error("cannot configure child output files: " +
                             std::string(std::strerror(error)));

  pid_t process = -1;
  error =
      posix_spawnp(&process, argv[0], &actions, nullptr, argv.data(), environ);
  if (error == ENOENT)
    return {127, {}, {}};
  if (error != 0)
    throw std::runtime_error("cannot start child process: " +
                             std::string(std::strerror(error)));

  int status = 0;
  pid_t waited;
  do {
    waited = waitpid(process, &status, 0);
  } while (waited < 0 && errno == EINTR);
  if (waited < 0)
    throw std::runtime_error("cannot wait for child process: " +
                             std::string(std::strerror(errno)));

  const int exit_code = WIFEXITED(status)     ? WEXITSTATUS(status)
                        : WIFSIGNALED(status) ? 128 + WTERMSIG(status)
                                              : 1;
  return {exit_code, read_file(stdout_path), read_file(stderr_path)};
}
#endif

std::string kind(const TypeRef &type) {
  if (!type)
    return "void";
  const auto unaliased = type->unalias();
  switch (unaliased->kind()) {
  case TypeKind::Integer:
    return "i32";
  case TypeKind::Real:
    return "f64";
  case TypeKind::Boolean:
    return "bool";
  case TypeKind::Array:
  case TypeKind::Record:
    return "address";
  default:
    return "void";
  }
}

enum class CliKind { Boolean, Integer, Real };
struct CliValue {
  CliKind kind;
  bool boolean = false;
  std::int32_t integer = 0;
  double real = 0.0;
};

std::int32_t parse_i32(std::string_view text) {
  bool explicit_plus = !text.empty() && text.front() == '+';
  if (explicit_plus) {
    text.remove_prefix(1);
    if (text.empty() || text.front() == '+' || text.front() == '-') {
      throw std::runtime_error("invalid integer argument");
    }
  }

  std::int64_t value = 0;
  const auto [end, error] =
      std::from_chars(text.data(), text.data() + text.size(), value);
  if (error != std::errc{} || end != text.data() + text.size() ||
      value < INT32_MIN || value > INT32_MAX) {
    throw std::runtime_error("integer argument is outside signed i32 range");
  }
  return static_cast<std::int32_t>(value);
}

double parse_real(std::string_view text) {
  if (text.empty())
    throw std::runtime_error("invalid real argument");
  size_t position = (text.front() == '+' || text.front() == '-') ? 1 : 0;
  if (position == text.size() || text[position] < '0' || text[position] > '9') {
    throw std::runtime_error("invalid real argument");
  }
  while (position < text.size() && text[position] >= '0' &&
         text[position] <= '9')
    ++position;
  if (position < text.size() && text[position] == '.') {
    ++position;
    while (position < text.size() && text[position] >= '0' &&
           text[position] <= '9')
      ++position;
  }
  if (position < text.size() &&
      (text[position] == 'e' || text[position] == 'E')) {
    ++position;
    if (position < text.size() &&
        (text[position] == '+' || text[position] == '-'))
      ++position;
    const size_t exponent_start = position;
    while (position < text.size() && text[position] >= '0' &&
           text[position] <= '9')
      ++position;
    if (position == exponent_start)
      throw std::runtime_error("invalid real argument");
  }
  if (position != text.size())
    throw std::runtime_error("invalid real argument");
  if (!text.empty() && text.front() == '+')
    text.remove_prefix(1);

  double value = 0.0;
  const auto [end, error] =
      std::from_chars(text.data(), text.data() + text.size(), value,
                      std::chars_format::general);
  if (error != std::errc{} || end != text.data() + text.size() ||
      !std::isfinite(value)) {
    throw std::runtime_error(
        "real argument is outside the finite f64 range or invalid");
  }
  return value;
}

std::string path_to_utf8(const std::filesystem::path &path) {
  const auto bytes = path.u8string();
  return {reinterpret_cast<const char *>(bytes.data()), bytes.size()};
}

CliValue parse_cli_value(const std::string &text) {
  if (text == "true" || text == "false") {
    return {CliKind::Boolean, text == "true"};
  }
  if (text.find_first_of(".eE") == std::string::npos) {
    return {CliKind::Integer, false, parse_i32(text)};
  }
  return {CliKind::Real, false, 0, parse_real(text)};
}

std::int32_t real_to_i32(double value) {
  const double rounded =
      std::round(value); // Halfway cases round away from zero.
  if (!std::isfinite(rounded) || rounded < INT32_MIN || rounded > INT32_MAX) {
    throw std::runtime_error(
        "real-to-integer conversion is outside signed i32 range");
  }
  return static_cast<std::int32_t>(rounded);
}

std::string encode_real(double value) {
  char buffer[64];
  const auto [end, error] = std::to_chars(
      buffer, buffer + sizeof(buffer), value, std::chars_format::general,
      std::numeric_limits<double>::max_digits10);
  if (error != std::errc{})
    throw std::runtime_error("cannot encode real argument");
  return {buffer, end};
}

std::string read_result(const std::filesystem::path &path) {
  const auto text = read_file(path);
  if (text.empty())
    return {};
  return text.back() == '\n' ? text.substr(0, text.size() - 1) : text;
}

WasmResult decode_result(const std::string &encoded,
                         const std::string &result_kind) {
  if (result_kind == "i32") {
    std::int32_t value = 0;
    const auto [end, error] =
        std::from_chars(encoded.data(), encoded.data() + encoded.size(), value);
    if (error != std::errc{} || end != encoded.data() + encoded.size())
      throw std::runtime_error("runner returned an invalid i32 result");
    return value;
  }
  if (result_kind == "bool") {
    if (encoded == "true")
      return true;
    if (encoded == "false")
      return false;
    throw std::runtime_error("runner returned an invalid boolean result");
  }
  if (result_kind == "address") {
    std::uint32_t value = 0;
    const auto [end, error] =
        std::from_chars(encoded.data(), encoded.data() + encoded.size(), value);
    if (error != std::errc{} || end != encoded.data() + encoded.size())
      throw std::runtime_error("runner returned an invalid address result");
    return value;
  }
  if (result_kind == "f64") {
    if (encoded == "NaN")
      return std::numeric_limits<double>::quiet_NaN();
    if (encoded == "Infinity")
      return std::numeric_limits<double>::infinity();
    if (encoded == "-Infinity")
      return -std::numeric_limits<double>::infinity();
    double value = 0.0;
    const auto [end, error] =
        std::from_chars(encoded.data(), encoded.data() + encoded.size(), value,
                        std::chars_format::general);
    if (error != std::errc{} || end != encoded.data() + encoded.size())
      throw std::runtime_error("runner returned an invalid f64 result");
    return value;
  }
  return std::monostate{};
}

std::vector<std::string> encode_arguments(
    const RoutineSymbol &routine, const std::vector<std::string> &arguments) {
  for (const auto &parameter : routine.parameters) {
    if (!parameter.type || !(parameter.type->is_integer() ||
                             parameter.type->is_real() ||
                             parameter.type->is_boolean())) {
      throw std::runtime_error("routine '" + routine.name +
                               "' has a non-primitive parameter '" +
                               parameter.name + "'");
    }
  }
  if (arguments.size() != routine.parameters.size()) {
    throw std::runtime_error("routine '" + routine.name + "' expects " +
                             std::to_string(routine.parameters.size()) +
                             " argument(s), but " +
                             std::to_string(arguments.size()) + " provided");
  }
  std::vector<std::string> encoded_arguments;
  encoded_arguments.reserve(arguments.size());
  for (std::size_t i = 0; i < arguments.size(); ++i) {
    const auto &parameter = routine.parameters[i];
    try {
      const CliValue value = parse_cli_value(arguments[i]);
      const std::string parameter_kind = kind(parameter.type);
      if (parameter_kind == "i32") {
        const auto converted =
            value.kind == CliKind::Boolean   ? (value.boolean ? 1 : 0)
            : value.kind == CliKind::Integer ? value.integer
                                             : real_to_i32(value.real);
        encoded_arguments.push_back("i32:" + std::to_string(converted));
      } else if (parameter_kind == "f64") {
        const double converted = value.kind == CliKind::Boolean
                                     ? (value.boolean ? 1.0 : 0.0)
                                 : value.kind == CliKind::Integer
                                     ? static_cast<double>(value.integer)
                                     : value.real;
        encoded_arguments.push_back("f64:" + encode_real(converted));
      } else if (value.kind == CliKind::Boolean) {
        encoded_arguments.push_back(value.boolean ? "bool:1" : "bool:0");
      } else if (value.kind == CliKind::Integer &&
                 (value.integer == 0 || value.integer == 1)) {
        encoded_arguments.push_back("bool:" + std::to_string(value.integer));
      } else if (value.kind == CliKind::Real) {
        throw std::runtime_error("real-to-boolean conversion is not allowed");
      } else {
        throw std::runtime_error("integer-to-boolean conversion requires 0 or 1");
      }
    } catch (const std::runtime_error &error) {
      throw std::runtime_error("argument " + std::to_string(i + 1) +
                               " ('" + arguments[i] + "') is invalid for parameter '" +
                               parameter.name + "' of type " +
                               parameter.type->to_string() + ": " + error.what());
    }
  }
  return encoded_arguments;
}

} // namespace

std::string validate_wasm_arguments(
    const RoutineSymbol &routine, const std::vector<std::string> &arguments) {
  try {
    encode_arguments(routine, arguments);
    return {};
  } catch (const std::runtime_error &error) {
    return error.what();
  }
}

WasmExecutionResult execute_wasm(std::string_view wat,
                                 const RoutineSymbol &routine,
                                 const std::vector<std::string> &arguments) {
  WasmExecutionResult result{1, std::monostate{}, {}, {}};
  try {
    const auto result_kind = kind(routine.return_type);
    const auto encoded_arguments = encode_arguments(routine, arguments);

    static std::atomic<unsigned long long> serial{0};
    std::random_device random;
    const auto temp_base = std::filesystem::temp_directory_path();
    std::filesystem::path temp_dir;
    bool created = false;
    for (int attempt = 0; attempt < 100; ++attempt) {
      temp_dir = temp_base / ("imperative-wasm-" + std::to_string(random()) +
                              "-" + std::to_string(serial.fetch_add(1)));
      std::error_code error;
      if (std::filesystem::create_directory(temp_dir, error)) {
        created = true;
#ifndef _WIN32
        std::filesystem::permissions(
            temp_dir, std::filesystem::perms::owner_all,
            std::filesystem::perm_options::replace, error);
        if (error) {
          std::error_code ignored;
          std::filesystem::remove_all(temp_dir, ignored);
          throw std::runtime_error("cannot secure temporary directory: " +
                                   error.message());
        }
#endif
        break;
      }
    }
    if (!created)
      throw std::runtime_error("cannot create unique temporary directory");
    struct TempDirectoryCleanup {
      std::filesystem::path path;
      ~TempDirectoryCleanup() {
        std::error_code ignored;
        std::filesystem::remove_all(path, ignored);
      }
    } cleanup{temp_dir};

    const auto wat_path = temp_dir / "module.wat";
    const auto wasm_path = temp_dir / "module.wasm";
    const auto tool_out = temp_dir / "tool.stdout";
    const auto tool_err = temp_dir / "tool.stderr";
    const auto runner_result = temp_dir / "result.txt";

    {
      std::ofstream wat_file(wat_path, std::ios::binary);
      wat_file.write(wat.data(), static_cast<std::streamsize>(wat.size()));
      if (!wat_file)
        throw std::runtime_error("cannot write temporary WAT file");
    }

    const std::string wat_utf8 = path_to_utf8(wat_path);
    const std::string wasm_utf8 = path_to_utf8(wasm_path);
    const std::string result_utf8 = path_to_utf8(runner_result);

    const auto assembled = run_process({"wat2wasm", wat_utf8, "-o", wasm_utf8},
                                       tool_out, tool_err);
    if (assembled.status == 127)
      throw std::runtime_error(
          "required dependency 'wat2wasm' was not found on PATH");
    if (assembled.status != 0)
      throw std::runtime_error("wat2wasm failed" +
                               (assembled.stderr_text.empty()
                                    ? std::string{}
                                    : ": " + assembled.stderr_text));

    const auto validated =
        run_process({"wasm-validate", wasm_utf8}, tool_out, tool_err);
    if (validated.status == 127)
      throw std::runtime_error(
          "required dependency 'wasm-validate' was not found on PATH");
    if (validated.status != 0)
      throw std::runtime_error("wasm-validate failed" +
                               (validated.stderr_text.empty()
                                    ? std::string{}
                                    : ": " + validated.stderr_text));

    std::vector<std::string> runner_args{"node",      IMP_WASM_RUNNER_PATH,
                                         wasm_utf8,   routine.name,
                                         result_kind, result_utf8};
    runner_args.insert(runner_args.end(), encoded_arguments.begin(),
                       encoded_arguments.end());
    const auto execution = run_process(runner_args, tool_out, tool_err);
    if (execution.status == 127)
      throw std::runtime_error(
          "required dependency 'node' was not found on PATH");

    result.stdout_text = execution.stdout_text;
    result.diagnostic = execution.stderr_text;
    result.exit_code = execution.status;
    if (execution.status != 0) {
      if (result.diagnostic.empty())
        result.diagnostic = "WASM execution failed";
      return result;
    }
    if (result_kind != "void") {
      const auto encoded = read_result(runner_result);
      if (encoded.empty())
        throw std::runtime_error("runner did not return a result");
      result.value = decode_result(encoded, result_kind);
    }
    return result;
  } catch (const std::exception &error) {
    result.exit_code = 1;
    result.diagnostic = error.what();
    return result;
  }
}
