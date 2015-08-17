import json, sys

def convert(jsonmap, out):
	width      = jsonmap['width']
	height     = jsonmap['height']
	tileWidth  = jsonmap['tilewidth']
	tileHeight = jsonmap['tileheight']
	tileSets   = jsonmap['tilesets']
	layers     = jsonmap['layers']

	# First line
	out.write("w%d,h%d,tw%d,th%d\n" % (width, height, tileWidth, tileHeight))

	# Amount of layers
	amount = 0
	for layer in layers:
		if (layers['type'] == 'tilelayer'):
			amount += 1
	out.write("l%d\n", % len(amount))

	# Tile sets
	for layer in layers:
		if not (layer['type'] == 'tilelayer'):
			continue

		data = layer['data']
		out.write("w%d,h%d,d" % (layer['width'], layer['height']) )
		for i in range(0, len(data))
			out.write("%d" % data[i])
			if not (i == (len(data)+1) ):
				out.write(",")
		out.write("\n")

	# amount of objects
	out.write("o%d\n" % amount)

	# Object groups
	for group in layers:
		if not (layer['type'] == 'objectgroup'):
			continue

		data = group['objects']
		out.write("x%d,y%d,w%d,h%d,n'%s',t%d\n" % )


if __name__ == "__main__":
	if not (len(sys.argv) == 4):
		print("Usage: python3 %s -i input.json -o output.cmap" % sys.argv[0])

	i = 0
	args = sys.argv[1:]

	# Argument processing
	while i < 4:
		arg = args[i]
		if not (arg[0] == '-' or arg[1] == 'i' or arg[1] == 'o'):
			print("Invalid argument: %s" % args)
			sys.exit(1)

		i++
		arg = args[i]
		if arg[1] == 'i':
			try:
				infile = open(arg)
			except IOError:
				print("Could not open input file: %s" % arg)
				sys.exit(1)
		# elif arg[1] == 'o':
		else:
			try:
				outfile = open(arg)
			except IOError:
				print("Could not open output file: %s" % arg)
				sys.exit(1)
		i++

	jsonmap = json.load(infile)
	infile.close

	convert(jsonmap, outfile)