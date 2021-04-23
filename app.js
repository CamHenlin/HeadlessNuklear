const fs = require('fs');
const buf = fs.readFileSync('./sample.wasm');
const { WASI } = require("wasi");
const wasi = new WASI();
const importObject = { wasi_snapshot_preview1: wasi.wasiImport };

const runTest = async () => {

	const wasm = await WebAssembly.compile(
		fs.readFileSync("./sample.wasm")
	);

	const instance = await WebAssembly.instantiate(wasm, importObject);

	console.log(`instance`);
	console.log(instance);

	wasi.start(instance);
}

runTest()