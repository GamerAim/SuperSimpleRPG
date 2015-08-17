import json, sys

def convert(jsonmap, out):
	width      = jsonmap['width']
	height     = jsonmap['height']
	tileWidth  = jsonmap['tilewidth']
	tileHeight = jsonmap['tileheight']
	tileSets   = jsonmap['tilesets']
	layers     = jsonmap['layers']

	# First line
	out.write( bytes("w%d,h%d,tw%d,th%d\n" % (width, height, tileWidth, tileHeight), 'UTF-8') )

	# Amount of layers
	amount = 0
	for layer in layers:
		if (layer['type'] == "tilelayer"):
			amount += 1
	out.write(bytes("l%d\n" % amount, 'UTF-8'))

	# Tile sets
	for layer in layers:
		if not (layer['type'] == "tilelayer"):
			continue

		data = layer['data']
		out.write(bytes("w%d,h%d,d" % (layer['width'], layer['height']), 'UTF-8') )
		for i in range(0, len(data)):
			out.write(bytes("%d" % data[i], 'UTF-8') )
			if not (i == len(data)-1 ):
				out.write(bytes(",", 'UTF-8') )
		out.write(bytes("\n", 'UTF-8') )

	# amount of objects
	out.write(bytes("o%d\n" % amount, 'UTF-8') )

	# Object groups
	for group in layers:
		if not (group['type'] == 'objectgroup'):
			continue

		data = group['objects']
		out.write(bytes("%d\n" % len(data), 'UTF-8'))
		for gr in data:
			out.write(bytes("x%d,y%d,w%d,h%d,n'%s',t%d\n" % \
			  (gr['x'], gr['y'], gr['width'], gr['height'], \
			  gr['name'], int(gr['type'])), 'UTF-8'))
	out.close()


if __name__ == "__main__":
	if not (len(sys.argv) == 5):
		print("Usage: python3 %s -i input.json -o output.cmap" % sys.argv[0])
		sys.exit(1)

	i       = 0
	outfile = None
	args    = sys.argv[1:]

	# Argument processing
	while i < 4:
		arg = args[i]
		if not (arg[0] == '-' or not(arg[1] == 'i' or arg[1] == 'o')):
			print("Invalid argument: %s" % arg)
			sys.exit(1)

		arg = args[i]
		if arg[1] == 'i':
			try:
				arg     = args[i+1]
				infile  = open(arg, 'r')
				jsonmap = json.loads(infile.read())
			except IOError:
				print("Could not open input file: %s" % arg)
				sys.exit(1)
		elif arg[1] == 'o':
			try:
				arg     = args[i+1]
				outfile = open(arg, 'wb')
			except IOError:
				print("Could not open output file: %s" % arg)
				sys.exit(1)
		else:
			print("Invalid argument")
			sys.exit(1)
		i += 2

	convert(jsonmap, outfile)