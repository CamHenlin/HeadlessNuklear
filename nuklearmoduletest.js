const Nuklear = require('./app.js')

setTimeout(async () => {

	const result = await Nuklear.runEvent(`D100,100`)

	console.log(`result:`)
	console.log(result)
}, 1000)