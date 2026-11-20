const assert = require('node:assert/strict');
const fs = require('node:fs');
const os = require('node:os');
const path = require('node:path');
const { execFileSync } = require('node:child_process');

function compileWat(compiler, wat, wasm) {
    const args = [wat, '-o', wasm];
    const header = fs.readFileSync(compiler).subarray(0, 80).toString();
    if (header.startsWith('#!') && header.includes('node')) {
        execFileSync(process.execPath, [compiler, ...args], { stdio: 'pipe' });
    } else {
        execFileSync(compiler, args, { stdio: 'pipe' });
    }
}

// Global arrays start with their declared length and zero values of each primitive type.
function testGlobalDefaults(api) {
    const memory = new DataView(api.memory.buffer);
    const reals = api.global0();
    const integers = api.global1();
    const flags = api.global2();
    assert.equal(new Set([reals, integers, flags]).size, 3);
    assert.equal(memory.getInt32(reals, true), 3);
    assert.equal(memory.getInt32(integers, true), 2);
    assert.equal(memory.getInt32(flags, true), 2);
    for (let i = 0; i < 3; ++i) {
        assert.equal(memory.getFloat64(reals + 8 + i * 8, true), 0);
    }
    for (const address of [integers, flags]) {
        for (let i = 0; i < 2; ++i) {
            assert.equal(memory.getInt32(address + 8 + i * 4, true), 0);
        }
    }
}

// Every execution of a local declaration creates a fresh zero-filled object.
function testFreshLocalObjects(api) {
    const first = api.new_local();
    new DataView(api.memory.buffer).setFloat64(first + 8, 99, true);
    const second = api.new_local();
    assert.notEqual(first, second);
    const memory = new DataView(api.memory.buffer);
    assert.equal(memory.getFloat64(first + 8, true), 99);
    assert.equal(memory.getFloat64(second + 8, true), 0);
    assert.equal(memory.getInt32(second, true), 3);
}

// Initializers share the same object, and checked indexing preserves f64 values.
function testSharedReferenceAndIndexing(api, reports) {
    assert.equal(api.global3(), api.global0());
    api.write_real(1, 2.75);
    api.write_real(3, -8.5);
    assert.equal(api.read_real(1), 2.75);
    assert.equal(api.read_real(2), 0);
    assert.equal(api.read_real(3), -8.5);
    for (const index of [-1, 0, 4, 2147483647]) {
        assert.throws(() => api.read_real(index), WebAssembly.RuntimeError);
        assert.deepEqual(reports.at(-1), { row: 42, column: 7, kind: 1 });
        assert.throws(() => api.write_real(index, 7), WebAssembly.RuntimeError);
        assert.deepEqual(reports.at(-1), { row: 42, column: 7, kind: 1 });
    }
    assert.equal(api.read_real(1), 2.75);
}

// Allocation aligns addresses, grows memory, and rejects invalid or overflowing sizes.
function testAllocation(api, reports) {
    const first = api.alloc(1n, 0, 0);
    const second = api.alloc(9n, 0, 0);
    assert.equal(first % 8, 0);
    assert.equal(second, first + 8);
    assert.equal(api.alloc(1n, 0, 0), second + 16);
    for (const bytes of [0n, -1n, 4294967296n, 4294967297n]) {
        assert.throws(() => api.alloc(bytes, 73, 11), WebAssembly.RuntimeError);
        assert.deepEqual(reports.at(-1), { row: 73, column: 11, kind: 2 });
    }
    const before = api.memory.buffer.byteLength;
    const large = api.alloc(65536n, 0, 0);
    assert.ok(api.memory.buffer.byteLength > before);
    assert.ok(large + 65536 <= api.memory.buffer.byteLength);
    assert.equal(new DataView(api.memory.buffer).getInt32(large, true), 0);
}

// A failed memory.grow traps without advancing the heap pointer.
function testGrowthFailure(api, reports) {
    const before = api.alloc(1n, 0, 0);
    assert.throws(() => api.alloc(65536n, 91, 12), WebAssembly.RuntimeError);
    assert.deepEqual(reports.at(-1), { row: 91, column: 12, kind: 2 });
    assert.equal(api.alloc(1n, 0, 0), before + 8);
}

// Local allocation reports its declaration location before the trap.
function testLocalAllocationLocation(api, reports) {
    assert.throws(() => api.fail_local(), WebAssembly.RuntimeError);
    assert.deepEqual(reports.at(-1), { row: 87, column: 6, kind: 2 });
}

// Reports remain available even when nested global construction aborts instantiation.
function testGlobalAllocationLocation(generator, compiler, wat, wasm) {
    execFileSync(generator, ['--write-failing-memory-fixture', wat], { stdio: 'pipe' });
    compileWat(compiler, wat, wasm);
    const reports = [];
    const imports = { host: { report_trap(row, column, kind) { reports.push({ row: row >>> 0, column: column >>> 0, kind }); } } };
    assert.throws(() => new WebAssembly.Instance(new WebAssembly.Module(fs.readFileSync(wasm)), imports), WebAssembly.RuntimeError);
    assert.deepEqual(reports, [{ row: 2, column: 5, kind: 2 }]);
}

const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'imperative-memory-'));
try {
    const [generator, wat2wasm] = process.argv.slice(2);
    const wat = path.join(directory, 'memory.wat');
    const wasm = path.join(directory, 'memory.wasm');
    execFileSync(generator, ['--write-memory-fixture', wat], { stdio: 'pipe' });
    compileWat(wat2wasm, wat, wasm);
    const reports = [];
    const imports = { host: { report_trap(row, column, kind) { reports.push({ row: row >>> 0, column: column >>> 0, kind }); } } };
    const api = new WebAssembly.Instance(new WebAssembly.Module(fs.readFileSync(wasm)), imports).exports;
    testGlobalDefaults(api);
    testFreshLocalObjects(api);
    testSharedReferenceAndIndexing(api, reports);
    testAllocation(api, reports);
    testLocalAllocationLocation(api, reports);

    const original = fs.readFileSync(wat, 'utf8');
    assert.ok(original.includes('(memory 1)'));
    fs.writeFileSync(wat, original.replace('(memory 1)', '(memory 1 1)'));
    compileWat(wat2wasm, wat, wasm);
    const limited = new WebAssembly.Instance(new WebAssembly.Module(fs.readFileSync(wasm)), imports).exports;
    testGrowthFailure(limited, reports);
    testGlobalAllocationLocation(generator, wat2wasm, wat, wasm);
    console.log('Memory runtime tests passed');
} finally {
    assert.equal(path.dirname(directory), path.resolve(os.tmpdir()));
    assert.ok(path.basename(directory).startsWith('imperative-memory-'));
    fs.rmSync(directory, { recursive: true, force: true });
}
