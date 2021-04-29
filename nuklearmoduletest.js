const Nuklear = require('./app.js')

setTimeout(async () => {

	const start = Date.now()
	const result = await Nuklear.runEvent(`D100,100`)
	const end = Date.now()

	console.log(`result:`)
	console.log(result)
	console.log(`timing:`)
	console.log(end - start)
}, 1000)