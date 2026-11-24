import fs from 'node:fs';

const [wasmPath, exportName, resultKind, resultPath, ...encodedArgs] = process.argv.slice(2);
const output = [];
let reportedTrap;

function formatReal(value) {
  if (Number.isNaN(value)) return 'NaN';
  if (value === Infinity) return 'Infinity';
  if (value === -Infinity) return '-Infinity';
  if (Object.is(value, -0)) return '-0.0';
  const fractionalMantissa = (text) => {
    const exponentAt = text.search(/[eE]/);
    const mantissa = exponentAt < 0 ? text : text.slice(0, exponentAt);
    const exponent = exponentAt < 0 ? '' : `e${Number(text.slice(exponentAt + 1))}`;
    return `${mantissa.includes('.') ? mantissa : `${mantissa}.0`}${exponent}`;
  };
  const fixed = fractionalMantissa(value.toString());
  const scientific = fractionalMantissa(value.toExponential());
  return scientific.length < fixed.length ? scientific : fixed;
}

const imports = {
  host: {
    report_trap(row, column, kind) {
      reportedTrap = { row: row >>> 0, column: column >>> 0, kind };
    },
  },
  env: {
    print_i32(value) { output.push(String(value | 0)); },
    print_f64(value) { output.push(formatReal(value)); },
    print_bool(value) { output.push((value | 0) === 0 ? 'false' : 'true'); },
    print_space() { output.push(' '); },
    print_newline() { output.push('\n'); },
  },
};

try {
  reportedTrap = undefined;
  const { instance } = await WebAssembly.instantiate(fs.readFileSync(wasmPath), imports);
  const fn = instance.exports[exportName];
  if (typeof fn !== 'function') throw new Error(`WASM export '${exportName}' is missing`);
  const args = encodedArgs.map((arg) => {
    const colon = arg.indexOf(':');
    const tag = arg.slice(0, colon), raw = arg.slice(colon + 1);
    if (tag === 'i32' || tag === 'bool') return Number(raw) | 0;
    if (tag === 'f64') return raw === 'NaN' ? NaN : raw === 'Infinity' ? Infinity : raw === '-Infinity' ? -Infinity : Number(raw);
    throw new Error(`invalid runner argument '${arg}'`);
  });
  reportedTrap = undefined;
  const value = fn(...args);
  let serialized = '';
  if (resultKind === 'i32') serialized = String(value | 0);
  else if (resultKind === 'bool') serialized = (value | 0) === 0 ? 'false' : 'true';
  else if (resultKind === 'address') serialized = String(value >>> 0);
  else if (resultKind === 'f64') serialized = Number.isNaN(value) ? 'NaN' : value === Infinity ? 'Infinity' : value === -Infinity ? '-Infinity' : Object.is(value, -0) ? '-0.0' : String(value);
  if (resultKind !== 'void') fs.writeFileSync(resultPath, `${serialized}\n`, 'utf8');
} catch (error) {
  const message = error?.message ?? error;
  const trapMessages = {
    1: 'array index out of bounds',
    2: 'memory allocation failed',
    3: 'invalid conversion',
    4: 'arithmetic error',
  };
  if (reportedTrap) {
    const { row, column, kind } = reportedTrap;
    const location = row === 0 ? '' : ` at ${row}:${column}`;
    process.stderr.write(`WASM runtime error${location}: ${trapMessages[kind] ?? message}\n`);
  } else {
    process.stderr.write(`WASM runtime error: ${message}\n`);
  }
  process.exitCode = 1;
} finally {
  process.stdout.write(output.join(''));
}
