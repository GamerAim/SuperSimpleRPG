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

	/*Height and width in pixels of for each cell*/
	int tileWidth;
	int tileHeight;

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
	int w;
	int h;

	/*object name and value*/
	char *name;
	int  value;
}object_t;

typedef struct {
	int objectsLen;
	object_t *objects;
}objectGroup_t;

typedef struct {
	/* The height and width present in this structure may not reflect each
	 * tileSet
	 */
	int tileSetsLen;
	tileSet_t *tileSets;

	/*tile[gid-1], contents are mapped to tileSets*/
	int tilesLen;
	tile_t *tiles;

	/*Layers*/
	int layersLen;
	mapLayer_t *layers;

	/*Objects*/
	
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

map_t *LoadMap()
{
	return NULL;
}






