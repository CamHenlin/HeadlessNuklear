'use strict';

global.test = ``;
global.returnValue = `initial`;

let commandList = []

global.storeCommandOnCommandList = (command) => {

	// TODO: we could add some command validation here
	console.log(`storeCommandOnCommandList: ${command}`)
	commandList.push(command)
}

global.flushCommandList = () => {
	console.log(`flushCommandList`)
	commandList = []
}

setTimeout(() => {
	console.log(`returnvalue is currently:`)
	console.log(commandList)
	global.returnValue = ``;
	global.test = 'M100,100'
}, 1000)
setTimeout(() => {
	console.log(`returnvalue is currently:`)
	console.log(commandList)
	global.returnValue = ``;
	global.test = 'D100,100'
}, 2000)
setTimeout(() => {
	console.log(`returnvalue is currently:`)
	console.log(commandList)
	global.returnValue = ``;
	global.test = 'KX '
}, 3000)
setTimeout(() => {
	console.log(`returnvalue is currently:`)
	console.log(commandList)
	global.returnValue = ``;
	global.test = 'KX '
}, 4000)
setTimeout(() => {
	console.log(`returnvalue is currently:`)
	console.log(commandList)
	global.returnValue = ``;
	global.test = 'KX '
}, 5000)
const test = require('./sample.js')

