Editor instruction (other instructions than these will be ignored; args in brackets are optional):

id mapId
	sets map id to mapId

author mapAuthor
	sets map author to mapAuthor
	
desc mapDescription
	sets map description to mapDescription (it should be the id of a valid text, not editable via map editor)
	
new width height tile
	creates new map with given size filled with given tile on layer 0

fill tile layer
	fills map with given tile on given layer
	
pick tile [layer]
	picks given tile on given layer (or keeps current layer if not specified); left-click on map to "paint", right-click to pick tile under pointer, middle-click to bucket fill with selected tile
	
deco decoId
	picks given deco; left-click on map to place deco, right-click to remove it
	
undo
	(partially working) undoes last action
	
grid
	toggles grid
	
mmap
	toggles minimap
	
save path
	saves map to given path
	
open mapId|path
	opens map, either searching the map with given id inside database or loading from specified file (eg. if map test is a map inside database, you can load it via command "open test" or "open data/cfg/maps/test.cfg", assuming that is the path of the map)
	
scshot path
	saves a .bmp screenshot of the map to given path

quit
	exits editor