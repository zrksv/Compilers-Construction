# Storage and memory contract

`WasmStorage` emits storage operations. Module assembly, expression generation,
and execution remain the responsibilities of their existing components.

## Module integration

- Use one fresh `WasmStorage` per module.
- Add `WasmStorage::memory_runtime()` once with `WatWriter::module_item()`.
  It defines the module's memory, `$heap`, and `$alloc (param i64) (result i32)`.
  Do not add another memory or use these internal names for source declarations.
- Call `begin_function()` before emitting each function, including the global
  initializer. It resets local bindings and `$tmpN` numbering, preserving globals.
- `emit_globals(program, model, globals, init_locals, init_instructions, emit_initializer)`
  emits globals and their initializers in source order. The same additional
  `init_locals` parameter is required by `emit_global_variable()`.
- Put `globals` at module scope. Put `init_locals` before `init_instructions` in
  one initialization function. Run that function once before the selected entry
  routine, for example using the module's `start` declaration. Do not rerun it
  before each routine call.
- Storage owns the `$tmpN` namespace. Writer-generated temporaries must use a
  different namespace, agreed as `$writer_tmpN`; the writer implementation is
  still pending.

## Values and creation

Initializer callbacks return WAT that leaves exactly one value on the stack,
already converted to the declared variable type. Storage evaluates the callback
once and stores its result. An aggregate initializer copies the existing i32
reference without allocating another object.

Without an initializer, primitives receive zero and arrays of primitives allocate
a fresh object each time their declaration executes. Records and arrays of
aggregates are not implemented yet and still report an internal generation error.
This is an implementation gap, not a restriction of the source language.

Array layout is a four-byte signed length at offset 0, four bytes of padding,
then elements at offset 8. Element stride is 8 for real and 4 for integer,
boolean, or aggregate references. Indexing starts at 1; checked access traps
outside `1..length`.

The allocator reserves address 0, starts at byte 8, aligns the end of every
allocation to 8 bytes, and grows memory in 64 KiB pages. Its request is i64 so
size calculations cannot wrap an i32 address before the capacity check. Zero
requests, sizes exceeding the memory32 address space, and growth failures trap.
Objects are never freed or reused. Fresh memory is zero-filled, so primitive
array elements already have their default values without a clearing loop.

## Checks

From the repository root, using the existing MSVC build directory:

```powershell
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure --timeout 30
```

`CodegenTest` checks storage emission. If `wat2wasm` from WABT and `node` are
available when CMake configures the project, it also registers
`CodegenMemoryRuntimeTest`. After adding tools to PATH, rerun `cmake -S . -B build`.
The runtime test builds a fixture using the storage emitters, compiles it with
WABT, and executes it in Node.js. It checks primitive defaults, distinct local
objects, shared references, f64 loads/stores, index traps, alignment, memory
growth, size traps, and growth failure. This tests the storage component;
the complete compiler-to-WASM path is still pending integration.
