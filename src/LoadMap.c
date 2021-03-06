typedef struct {
	/* x and y offset to the upper left corner of tile*/
	int x;
	int y;

	/*height and width of tile*/
	int height;
	int width;

	/*Offset to tile set*/
	int tileSet;

	/* Pointer to the correct surface from the tile set, but not a copy
	 * that needs to be freed*/
	SDL_Surface *image;
}tile_t;

typedef struct {
	/*Starting Global ID*/
	int startingGID;

	/*The dimensions of the physical image*/
	int imageWidth;
	int imageHeight;

	/*Height and width of each tile*/
	int tileWidth;
	int tileHeight;

	/*Tile set name*/
	char *name;

	/*SDL surface for tileSet*/
	SDL_Surface *image;
}tileSet_t;

typedef struct {
	/*cell height and width*/
	int width;
	int height;

	/* layer[y][x] where:
	 *	0 <= y < height
	 *	0 <= x < width
	*/
	int **layer;
}mapLayer_t;

typedef struct {
	/*x and y map offset*/
	int x;
	int y;

	/*height and width*/
	int width;
	int height;

	/*object name and value*/
	char *name;
	int  value;
}object_t;

typedef struct {
	int objectsLen;
	object_t *objects;
}objectGroup_t;

typedef struct {
	/*Height and width in cells of map*/
	int width;
	int height;

	/*Pixel length of each tile*/
	int tileWidth;
	int tileHeight;

	/*Contains surface info for tiles and individual tile info*/
	int tileSetsLen;
	tileSet_t *tileSets;

	/*tile[gid-1], contents are mapped to tileSets*/
	int tilesLen;
	tile_t *tiles;

	/*Layers*/
	int layersLen;
	mapLayer_t *layers;

	/*Objects*/
	int objectGroupsLen;
	objectGroup_t *objectGroups;
}map_t;

/* Map format (%d means integer, and %s means string):
 * First line: "w%d,h%d,tw%d,th%d"
 * 	w  = map cell width
 * 	h  = map cell height
 * 	tw = cell tile width
 * 	th = cell tile height
 * 
 * Second line: "t%d"
 * 	t  = amount of tile sets
 * 
 * Following lines are equal to the number of tilesets:
 * 	Formated as "w%d,h%d,tw%d,th%d,g%d,n'%s'":
 * 		w  = image width
 * 		h  = image height
 * 		tw = tile width
 * 		th = tile height
 * 		g  = first gid in tile set
 * 		n  = file name
 * 
 * Next line: "l%d"
 * 	l = amount of layers
 * 
 * Following lines are equal to the number of layers:
 * 	Formated as "w%d,h%d,d%d,%d,%d,%d,...":
 * 		w = width in cells
 * 		h = height in cells
 * 		d = gid values equal to w*h, seperated by a comma as, but not
 * 		    ending or starting with one. Example:
 * 			%d,%d,%d,%d
 *
 * Next line: "o%d"
 * 	o = amount of objects
 *
 * The following sets of lines are equal to the number of objects:
 * 	An integer for the number of lines following it
 *	 	Formated as x%d,y%d,w%d,h%d,v%d,n'%s':
 * 			x = x offset to upper left corner
 * 			y = y offset to upper left corner
 * 			w = width of object
 * 			h = height of object
 * 			v = integer value for object for game purposes
 * 			n = name of object
 *
 * Lastly, a line break and EOF
 */



/*local def*/
#define MAXNUMLEN 32
#define MAXSTRINGLEN 128

int dataLen;

int getNum(char *data, int *offset)
{
	int i, ret;

	ret = 0;
	for (i = (*offset); i < MAXNUMLEN && i < dataLen; i++) {
		if (data[i] < '0' || data[i] > '9')
			break;
		else {
			ret *= 10;
			ret += data[i] - '0';
		}
	}

	if (data[i] != ',' || data[i] != '\n')
		return -1;
	if ( i == (dataLen-1) && data[i] != '\n')
		return -1;

	*offset = i;
	return ret;
}

char *getString(char *data, int *offset)
{
	char *ret;
	int i, len;

	for (i = (*offset); i < MAXSTRINGLEN && i < dataLen; i++)
		if (data[i] == '\'')
			break;

	if ( i == (dataLen-1) && data[i] != '\n')
		return NULL;

	len = i - (*offset);
	if (len <= 0)
		return NULL;

	ret = calloc(len+1, sizeof(char));
	if (ret == NULL)
		return NULL;

	strncpy(ret, data+(*offset), len);

	*offset = i;
	return ret;
}

map_t *LoadMap(char *name)
{
	int tmp;
	char *map;
	char *num;
	map_t *ret;
	int offset;
	int64_t mapLen;
	int i, j, x, y;
	SDL_RWops *file;
	int width, height;

	/*Open file and load into memory*/
	file = SDL_RWFromFile(name, "rb");
	if (file == NULL)
		die("Failed to open file %s because %s\n", SDL_GetError());

	mapLen = SDL_RWsize(file);
	if (mapLen < 0) {
		if (SDL_RWclose(file) < 0)
			die("Failed to close file because %s\n", SDL_GetError());
		return NULL;
	}
	else if (mapLen == 0) {
		if (SDL_RWclose(file) < 0)
			die("Failed to close file because %s\n", SDL_GetError());

		/*Zero length files are invalid for this format.*/
		return NULL;
	}

	map = calloc(mapLen, sizeof(char));
	if (map == NULL) {
		if (SDL_RWclose(file) < 0)
			die("Failed to close file because %s\n", SDL_GetError());
		return NULL;
	}

	if (SDL_RWread(file, map, sizeof(char), mapLen) != mapLen) {
		free(map);
		if (SDL_RWclose(file) < 0)
			die("Failed to close file because %s\n", SDL_GetError());
		return NULL;
	}

	if (SDL_RWclose(file) < 0)
		die("Failed to close file because %s\n", SDL_GetError());

	/* So the functions that common use the map buffer won't overrun
	 * on the event of a file corruption
	 */
	dataLen = mapLen;

	/*Initialize variables, to shut gcc up*/
	num    = NULL;
	ret    = NULL;
	offset = 0;


	/*Start the memory allocation process for return variables*/
	num = calloc(MAXNUMLEN, sizeof(char));
	if (num == NULL)  {
		free(map);
		return NULL;
	}
	ret = calloc(sizeof(map_t), 1);
	if (ret == NULL)  {
		free(map);
		free(num);
		return NULL;
	}


	/*Parse header*/
	offset += 1;
	ret->width  = getNum(map, &offset);
	if (ret->width < 0)
		goto headererr;
	offset += 2;

	ret->height = getNum(map, &offset);
	if (ret->height < 0)
		goto headererr;
	offset += 3;

	ret->tileWidth = getNum(map, &offset);
	if (ret->tileWidth < 0)
		goto headererr;
	offset += 3;

	ret->tileHeight = getNum(map, &offset);
	if (ret->tileHeight < 0)
		goto headererr;
	offset += 1;


	/*Parse tileset stuff*/
	offset += 1;
	ret->tileSetsLen = getNum(map, &offset);
	if (ret->tileSetsLen < 0)
		goto headererr;
	else if (ret->tileSetsLen == 0)
		goto headererr;
	offset += 1;

	ret->tileSets = calloc(sizeof(tileSet_t), ret->tileSetsLen);
	if (ret->tileSets == NULL)
		goto headererr;

	/*Load tile sets into memory*/
	for (i = 0; i < ret->tileSetsLen; i++) {
		offset++;
		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto tileseterr;
		ret->tileSets[i].imageWidth = tmp;
		offset += 2;

		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto tileseterr;
		ret->tileSets[i].imageHeight = tmp;
		offset += 3;

		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto tileseterr;
		ret->tileSets[i].tileWidth = tmp;
		offset += 3;

		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto tileseterr;
		ret->tileSets[i].tileHeight = tmp;
		offset += 2;

		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto tileseterr;
		ret->tileSets[i].startingGID = tmp;
		offset += 3;

		ret->tileSets[i].name = getString(map, &offset);
		if (ret->tileSets[i].name == NULL)
			goto tileseterr;
		offset += 1;

		char fpath[MAXSTRINGLEN*2];
		snprintf(fpath, MAXSTRINGLEN*2, "assets/%s", \
			 ret->tileSets[i].name);
		ret->tileSets[i].image = IMG_Load(fpath);
		if (ret->tileSets[i].image == NULL)
			goto tileseterr;
		offset += 2;
	}


	/*Now to make the tile array*/
	int total = 0;
	for (i = 0; i < ret->tileSetsLen; i++) {
		width  = ret->tileSets[i].imageWidth;
		height = ret->tileSets[i].imageHeight;
		width  = width  % ret->tileSets[i].tileWidth;
		height = height % ret->tileSets[i].tileHeight;

		if ((ret->tileSets[i].imageWidth % ret->tileSets[i].tileWidth) != 0)
			if (ret->tileSets[i].tileWidth < ret->tileSets[i].imageWidth)
				width++;
		if ((ret->tileSets[i].imageHeight % ret->tileSets[i].tileHeight) != 0)
			if (ret->tileSets[i].tileHeight < ret->tileSets[i].imageHeight)
				height++;

		total += width * height;
	}

	assert(total > 0);
	ret->tilesLen = total;
	ret->tiles = calloc(sizeof(tile_t), total);
	if (ret->tiles == NULL)
		goto tileseterr;

	for (i = 0; i < ret->tileSetsLen; i++) {
		width  = ret->tileSets[i].imageWidth;
		height = ret->tileSets[i].imageHeight;
		width  = width  % ret->tileSets[i].tileWidth;
		height = height % ret->tileSets[i].tileHeight;

		if ((ret->tileSets[i].imageWidth % ret->tileSets[i].tileWidth) != 0)
			if (ret->tileSets[i].tileWidth < ret->tileSets[i].imageWidth)
				width++;
		if ((ret->tileSets[i].imageHeight % ret->tileSets[i].tileHeight) != 0)
			if (ret->tileSets[i].tileHeight < ret->tileSets[i].imageHeight)
				height++;

		int current = 0;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++) {
				ret->tiles[current].x      = width  * ret->tileSets[i].tileWidth;
				ret->tiles[current].y      = height * ret->tileSets[i].tileHeight;

				ret->tiles[current].width  = ret->tileSets[i].tileWidth;
				ret->tiles[current].height = ret->tileSets[i].tileHeight;

				ret->tiles[current].tileSet = i;
				ret->tiles[current].image   = ret->tileSets[i].image;
			}
	}


	/*Now to fetch the map layers*/
	offset++;
	ret->layersLen = getNum(map, &offset);
	if (ret->layersLen < 0)
		goto tileseterr;
	offset++;

	ret->layers = calloc(sizeof(mapLayer_t), ret->layersLen);
	if (ret->layers == NULL)
		goto tileseterr;

	/*Iterate over each layer line and load into memory*/
	for (i = 0; i < ret->layersLen; i++) {
		offset++;
		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto layerserr;
		ret->layers[i].width = tmp;
		offset += 2;

		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto layerserr;
		ret->layers[i].height = tmp;
		offset += 2;

		int **dPtr;
		dPtr = calloc(sizeof(int**), ret->layers[i].height);
		if (dPtr == NULL)
			goto layerserr;
		for (j = 0; j < ret->layers[i].height; j++) {
			dPtr[j] = calloc(sizeof(int*), ret->layers[i].width);
			if (dPtr[j] == NULL)
				goto layerserr;
		}
		ret->layers[i].layer = dPtr;

		int k;
		for (j = 0; j < ret->layers[i].height; j++) {
			for (k = 0; k < ret->layers[i].width; k++) {
				tmp = getNum(map, &offset);
				if (tmp < 0)
					goto layerserr;
				offset++;

				ret->layers[i].layer[j][k] = tmp;
			}
		}
	}

	/*Now to put all objects into memory*/
	offset++;
	tmp = getNum(map, &offset);
	if (tmp < 0)
		goto objectserr;
	ret->objectGroupsLen = tmp;
	offset++;

	ret->objectGroups = calloc(sizeof(objectGroup_t), ret->objectGroupsLen);
	if (ret->objectGroups == NULL)
		goto objectserr;

	/*Now Iterate over each set of objects*/
	for (i = 0; i < ret->objectGroupsLen; i++) {
		offset++;
		tmp = getNum(map, &offset);
		if (tmp < 0)
			goto objectserr;
		ret->objectGroups[i].objectsLen = tmp;
		offset++;

		for (j = 0; j < ret->objectGroups[i].objectsLen; j++) {
			offset++;
			tmp = getNum(map, &offset);
			if (tmp < 0)
				goto objectserr;
			ret->objectGroups[i].objects[j].x = tmp;
			offset += 2;

			tmp = getNum(map, &offset);
			if (tmp < 0)
				goto objectserr;
			ret->objectGroups[i].objects[j].y = tmp;
			offset += 2;

			tmp = getNum(map, &offset);
			if (tmp < 0)
				goto objectserr;
			ret->objectGroups[i].objects[j].width = tmp;
			offset += 2;

			tmp = getNum(map, &offset);
			if (tmp < 0)
				goto objectserr;
			ret->objectGroups[i].objects[j].height = tmp;
			offset += 2;

			tmp = getNum(map, &offset);
			if (tmp < 0)
				goto objectserr;
			ret->objectGroups[i].objects[j].value = tmp;
			offset += 3;

			ret->objectGroups[i].objects[j].name = getString(map, &offset);
			if (ret->objectGroups[i].objects[j].name == NULL)
				goto objectserr;
			offset++;
		}
	}

	/*Clean up and return*/
	free(map);
	free(num);
	return ret;


	/*error section of code*/
	objectserr:
		if (ret->objectGroups != NULL) {
			for (i = 0; i < ret->objectGroupsLen; i++) {
				if (ret->objectGroups[i].objects != NULL)
					for (j = 0; j < ret->objectGroups[i].objectsLen; j++)
						if (ret->objectGroups[i].objects[j].name != NULL)
							free(ret->objectGroups[i].objects[j].name);
					free(ret->objectGroups[i].objects);
			}
			free(ret->objectGroups);
		}

	layerserr:
		if (ret->layers != NULL) {
			if (ret->layersLen == 0)
				free(ret->layers);
			else {
				for (i = 0; i < ret->layers[i].height; i++)
					for (j = 0; ret->layers[i].layer[j] != NULL; j++)
						free(ret->layers[i].layer[j]);

				free(ret->layers);
			}
		}
	tileseterr:
		if (ret->tiles != NULL)
			free(ret->tiles);
		if (ret->tileSetsLen < 0)
			goto headererr;
		for (i = 0; i < ret->tileSetsLen; i++) {
			if (ret->tileSets[i].name != NULL)
				free(ret->tileSets[i].name);
			if (ret->tileSets[i].image != NULL)
				SDL_FreeSurface(ret->tileSets[i].image);
		}
	headererr:
		free(map);
		free(num);
		free(ret);
		return NULL;
}

void FreeMap(map_t *map)
{
	int i, j;

	if (map->objectGroups != NULL) {
		for (i = 0; i < map->objectGroupsLen; i++) {
			if (map->objectGroups[i].objects != NULL)
				for (j = 0; j < map->objectGroups[i].objectsLen; j++)
					if (map->objectGroups[i].objects[j].name != NULL)
						free(map->objectGroups[i].objects[j].name);
				free(map->objectGroups[i].objects);
		}
		free(map->objectGroups);
	}

	if (map->layers != NULL) {
		if (map->layersLen == 0)
			free(map->layers);
		else {
			for (i = 0; i < map->layers[i].height; i++)
				for (j = 0; map->layers[i].layer[j] != NULL; j++)
					free(map->layers[i].layer[j]);
			free(map->layers);
		}
	}

	if (map->tiles != NULL)
		free(map->tiles);

	if (map->tileSetsLen < 0)
		goto end;

	for (i = 0; i < map->tileSetsLen; i++) {
		if (map->tileSets[i].name != NULL)
			free(map->tileSets[i].name);
		if (map->tileSets[i].image != NULL)
			SDL_FreeSurface(map->tileSets[i].image);
	}

	end:
		free(map);
}

#undef MAXNUMLEN
#undef MAXSTRINGLEN