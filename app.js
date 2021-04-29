'use strict';

const test = require('./sample.js')

// these are shared values that we use in C code
global.test = ``;

let commandList = []
let commandCache = {}
let commandCacheCounter = 0
let commandCallbackFunction = () => {} // placeholder, should never be called

// called from C code
global.storeCommandOnCommandList = (command) => {

	commandList.push(command)
}

const updateCommandCacheAndUpdateCommandListToCachedValues = () => {

	commandList = commandList.map((command) => {

		if (commandCache[command]) {

			return commandCache[command]
		}

		commandCache[command] = commandCacheCounter++

		return command
	})
}

// called from C code
global.flushCommandList = async () => {

	if (commandList.length === 0) {

		return
	}

	updateCommandCacheAndUpdateCommandListToCachedValues()

	if (commandCallbackFunction) {

		commandCallbackFunction()
	}

	commandList = []
}

class Nuklear {

  static async runEvent (event) {

  	console.log(`runEvent: ${event}`)

  	global.test = event

  	const output = await new Promise((resolve) => {

  		commandCallbackFunction = () => {

  			resolve(commandList)
  		}

  		test._EventLoop()
  	})

  	return output
  }
}

module.exports = Nuklear