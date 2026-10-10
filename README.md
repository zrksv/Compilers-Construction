# Compiler Construction: Imperative Language

**Team:** Fair Enough  
**Members:** Anna Zyrianova, Leonid Gunko, Egor Khramtsov  
**Course:** Compiler Construction (Innopolis University)  

This repository contains a full compiler pipeline for the **Imperative Language** (Project I). The compiler translates source `.imp` files into an Abstract Syntax Tree (AST) as a stepping stone toward generating WebAssembly (WASM) bytecode.

## 🛠 Architecture & Tech Stack

* **Source Language:** Imperative Language (Statically typed, reference semantics for aggregates).
* **Implementation Language:** C++20
* **Lexer (Scanner):** Hand-written C++ lexer (no Flex dependency for better cross-platform compatibility).
* **Parser:** `Bison` (LALR(1) parser generator) with `%skeleton "lalr1.cc"`.
* **AST:** Object-oriented C++ Abstract Syntax Tree with Visitor pattern.
* **Build System:** CMake (Modular architecture).

---

## 🚀 How to Build

### Prerequisites
* A modern C++ compiler supporting **C++20** (GCC, Clang, or MSVC).
* **CMake** (v3.29+).
* **Bison** (v3.5+). *On Ubuntu/Debian, install via `sudo apt install bison`*.

### Build Instructions
The project uses an out-of-source build. Run the following commands in the root directory:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

## 💻 How to Run (CLI Interface)

After building, the main compiler executable (`Compilers_Construction` or `fair_compiler`) will be located in the `build/` directory.

**General syntax:**
```bash
./Compilers_Construction <source.imp> [--tokens] [routine_name] [args...]
```

**1. Parse and print the AST (Default mode):**
Parses the file and outputs the formatted AST tree.
```bash
./Compilers_Construction ../tests/samples/01_print.imp
```

**2. Print tokens only (Lexer debug mode):**
Skips the parser and outputs the list of tokens recognized by the lexical analyzer.
```bash
./Compilers_Construction ../tests/samples/01_print.imp --tokens
```

---

## 🧪 Testing

We have built a comprehensive test suite to ensure the stability of the compiler. All executables are located in the `build/` directory.

1. **Test Lexer & SourceManager** (Unit tests for word, number, and operator recognition):
   ```bash
   ./lexer_tests
   ```
2. **Test App CLI** (Checks invalid arguments, non-existent files, empty files, and syntax error handling):
   ```bash
   ./tests/test_app
   ```
3. **Test Full Pipeline** (Integration tests passing `.imp` files through SourceManager -> Lexer -> Parser -> AST Printer):
   ```bash
   ./tests/test_pipeline
   ```

---

## 📁 Project Structure

* `src/common/` — File reading and location tracking (`SourceManager`, `Location`).
* `src/lexer/` — Hand-written lexical analyzer (`Lexer`, `Token`).
* `src/ast/` — AST node classes and Tree Printer (`AST`, `ASTPrinter`).
* `src/parser/` — Bison grammar rules and the C++ adapter (`Parser.yy`, `ParserDriver`).
* `src/app/` — Main CLI entry point (`main.cpp`).
* `tests/` — Test utilities, integration test runners, and `.imp` sample files.
