const fs = require('fs');
const buf = fs.readFileSync('./sample.wasm');
const { WASI } = require("wasi");
const { closeSync, openSync, readFileSync, writeFileSync } = require('fs');

const stdin = function writeToStdIn(buf) {
    return function() {
        if (!buf.length) {
            return null;
        }
        const c = buf[0];
        buf = buf.slice(1);
        return c;
    };
};
// stdout is a function which accepts characters
const stdout = function (char) {
    console.log(char);
};


const wasi = new WASI();
const importObject = { wasi_snapshot_preview1: wasi.wasiImport };

const runTest = async () => {

	const wasm = await WebAssembly.compile(
		fs.readFileSync("./sample.wasm")
	);


	wasm.stdout = stdout;
	wasm.stdin = stdin;

	const instance = await WebAssembly.instantiate(wasm, importObject)

	console.log(`instance`);
	console.log(instance);

	setTimeout(() => {

		wasi.start(instance);
	})

	console.log(`here`);
	console.log(instance);

	setTimeout(() => {

		console.log(`mouse`);
		stdin("M10,10")
	})
}

runTest()