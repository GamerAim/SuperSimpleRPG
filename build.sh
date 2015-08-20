#!/bin/bash

python3 src/json2cmap.py -i assets/map.json -o assets/map.cmap
gcc -lSDL2 -lSDL2_image -Os -Wall -g -Isrc/ src/main.c -o game

exit 0
