#pragma once
#include <libtcod.hpp>
// WEIRD THINGS
enum mapType { ice, barren };
enum noiseType {snow};

// Forward decs:
class cPlayer;
class entity;
class entityList;
class cellFile;

struct mapTile {
    unsigned char graphic;
	TCODColor backColor;
	TCODColor foreColor;
	bool isTransparent;
	bool isExplored;
    bool isWalkable;

    mapTile () {
        graphic = '?';
        backColor = TCODColor::black;
        foreColor = TCODColor::black;
        isTransparent = true;
        isExplored = false;
        isWalkable = false;
    }
};

typedef mapTile mapArray[80][50];

class mapData {
public:
    mapData();
    mapData(TCODMap *myFOV);
    ~mapData();

    void genNewMap();
    void drawRoom(int x, int y, int w, int h);
    void noise(noiseType newNoise, TCODConsole *myDest, cPlayer *mainPlayer);
    void clearMap();
    void randChanges(cPlayer *mainPlayer);

    bool changed;
    bool loaded;
    int mapLevel;

    // Master array of the map
    mapArray mapHandlerMaster;
    // Master list of entities
    entityList *masterList;
    // Reads mapdata from a file
    cellFile *fileHandler;
    // Points to the player's TCODFoV data
    TCODMap *playerFOVMap;

    // The current map type (woods, brick, whatever)
    mapType myType;
};