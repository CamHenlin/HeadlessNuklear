const Nuklear = require('./app.js')

setTimeout(async () => {

	const start = Date.now()
	let result = await Nuklear.runEvent(`M100,100`)
	console.log(result)
	result = await Nuklear.runEvent(`D100,100`)
	console.log(result)
	result = await Nuklear.runEvent(`U100,100`)
	console.log(result)
	result = await Nuklear.runEvent(`KX `)
	console.log(result)
	result = await Nuklear.runEvent(`KX `)
	console.log(result)
	result = await Nuklear.runEvent(`KX `)
	console.log(result)
	const end = Date.now()

	console.log(`result:`)
	console.log(`timing:`)
	console.log(end - start)
}, 1000)