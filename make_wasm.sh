clear; emcc -O0 Sample.c -s WASM=1 -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_handleString']" -o sample.js