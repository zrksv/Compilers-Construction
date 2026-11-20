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

function memory(api) { return new DataView(api.memory.buffer); }
function pointer(api, base, offset) { return memory(api).getUint32(base + offset, true); }
function element(api, base, index) { return pointer(api, base, 8 + (index - 1) * 4); }
function serial(api, leaf) { return memory(api).getInt32(leaf, true); }
function weight(api, leaf) { return memory(api).getFloat64(leaf + 8, true); }

// Global records, record arrays, and nested arrays recursively create distinct children.
function testRecursiveDefaults(api) {
    const box = api.box();
    assert.equal(api.alias(), box);
    assert.equal(memory(api).getInt32(box, true), 0);
    const child = pointer(api, box, 4);
    const items = pointer(api, box, 8);
    const matrix = pointer(api, box, 12);
    assert.equal(pointer(api, box, 16), api.seed());
    assert.ok(pointer(api, box, 20) > 0);
    assert.equal(memory(api).getInt32(items, true), 2);
    assert.equal(memory(api).getInt32(matrix, true), 2);
    const leaves = [child, element(api, items, 1), element(api, items, 2)];
    assert.equal(new Set(leaves).size, 3);
    for (const leaf of leaves) {
        assert.equal(weight(api, leaf), 3);
        assert.equal(memory(api).getInt32(leaf + 16, true), 0);
    }
    const rows = [element(api, matrix, 1), element(api, matrix, 2)];
    assert.notEqual(rows[0], rows[1]);
    for (const row of rows) {
        assert.equal(memory(api).getInt32(row, true), 2);
        assert.equal(memory(api).getInt32(row + 8, true), 0);
        assert.equal(memory(api).getInt32(row + 12, true), 0);
    }
    const first = element(api, api.boxes(), 1);
    const second = element(api, api.boxes(), 2);
    assert.notEqual(first, second);
    assert.notEqual(pointer(api, first, 4), pointer(api, second, 4));
    assert.notEqual(pointer(api, first, 8), pointer(api, second, 8));
    assert.equal(pointer(api, first, 16), api.seed());
    assert.equal(pointer(api, second, 16), api.seed());
}

// Side-effecting field initializers execute for each object in declaration/element order.
function testFieldInitializers(api) {
    assert.equal(api.ticks(), 10);
    assert.equal(serial(api, api.seed()), 1);
    const box = api.box();
    assert.equal(serial(api, pointer(api, box, 4)), 2);
    const items = pointer(api, box, 8);
    assert.equal(serial(api, element(api, items, 1)), 3);
    assert.equal(serial(api, element(api, items, 2)), 4);
    const first = element(api, api.boxes(), 1);
    const second = element(api, api.boxes(), 2);
    assert.equal(serial(api, pointer(api, first, 4)), 5);
    assert.equal(serial(api, pointer(api, second, 4)), 8);
}

// Local record aliases support mixed field/index assignments without changing other objects.
function testLocalRecords(api) {
    const before = api.ticks();
    const first = api.create();
    assert.equal(api.ticks(), before + 3);
    assert.equal(weight(api, pointer(api, first, 4)), 9.5);
    const items = pointer(api, first, 8);
    assert.equal(serial(api, element(api, items, 2)), 77);
    const matrix = pointer(api, first, 12);
    assert.equal(memory(api).getInt32(element(api, matrix, 1) + 12, true), 42);
    assert.equal(memory(api).getInt32(element(api, matrix, 2) + 12, true), 0);
    assert.equal(pointer(api, first, 16), api.seed());
    const second = api.create();
    assert.equal(api.ticks(), before + 6);
    assert.notEqual(first, second);
    assert.notEqual(pointer(api, first, 4), pointer(api, second, 4));
    assert.equal(weight(api, pointer(api, api.box(), 4)), 3);
}

// Field reads, writes, and aggregate-field assignment preserve shared references.
function testFieldAccessAndSharing(api) {
    assert.equal(api.read_weight(), 3);
    api.write_weight();
    assert.equal(api.read_weight(), 6.25);
    assert.equal(weight(api, pointer(api, api.alias(), 4)), 6.25);
    const before = api.ticks();
    api.replace_child();
    assert.equal(api.ticks(), before);
    assert.equal(pointer(api, api.box(), 4), api.seed());
    assert.equal(api.read_weight(), 3);
    assert.equal(api.read_item(1), 3);
    assert.equal(api.read_item(2), 4);
    for (const index of [-1, 0, 3]) {
        assert.throws(() => api.read_item(index), WebAssembly.RuntimeError);
    }
}

// An aggregate parameter can mutate its object; rebinding the parameter leaves the caller's reference intact.
function testParameterRebinding(api) {
    const local = api.create();
    const child = pointer(api, local, 4);
    api.rebind(local);
    assert.equal(pointer(api, local, 4), child);
    assert.equal(weight(api, child), 11.5);
    assert.equal(memory(api).getInt32(local, true), 0);
    assert.equal(memory(api).getInt32(api.box(), true), 123);
}

// Empty records have distinct aligned addresses on every creation.
function testEmptyRecords(api) {
    const first = api.new_empty();
    const second = api.new_empty();
    assert.ok(first > 0 && second > 0);
    assert.notEqual(first, second);
    assert.equal(first % 8, 0);
    assert.equal(second % 8, 0);
}

const directory = fs.mkdtempSync(path.join(os.tmpdir(), 'imperative-records-'));
try {
    const [generator, compiler] = process.argv.slice(2);
    for (const optimized of [false, true]) {
        const wat = path.join(directory, 'records.wat');
        const wasm = path.join(directory, 'records.wasm');
        const option = optimized ? '--write-optimized-record-fixture' : '--write-record-fixture';
        execFileSync(generator, [option, wat], { stdio: 'pipe' });
        compileWat(compiler, wat, wasm);
        const reports = [];
        const imports = { host: { report_trap(row, column, kind) { reports.push({ row: row >>> 0, column: column >>> 0, kind }); } } };
        const api = new WebAssembly.Instance(new WebAssembly.Module(fs.readFileSync(wasm)), imports).exports;
        testRecursiveDefaults(api);
        testFieldInitializers(api);
        testLocalRecords(api);
        testFieldAccessAndSharing(api);
        testParameterRebinding(api);
        testEmptyRecords(api);
        assert.equal(reports.length, 3);
        for (const report of reports) {
            assert.ok(report.row > 0 && report.column > 0);
            assert.equal(report.kind, 1);
        }
    }
    console.log('Record runtime tests passed with and without optimization');
} finally {
    assert.equal(path.dirname(directory), path.resolve(os.tmpdir()));
    assert.ok(path.basename(directory).startsWith('imperative-records-'));
    fs.rmSync(directory, { recursive: true, force: true });
}
