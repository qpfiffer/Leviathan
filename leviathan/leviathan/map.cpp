#include "map.h"
#include "main.h"
#include "entities.h"
#include "file_handler.h"

// CONSTRUCTORS AND CLASS FUNCTIONS
mapData::mapData() {
    changed = false;
    loaded = false;
    mapLevel = 1;

    masterList = new entityList;
    fileHandler = new cellFile;
}

mapData::mapData(TCODMap *myFOV) {
    changed = false;
    loaded = false;
    mapLevel = 1;

    masterList = new entityList;
    fileHandler = new cellFile;

    playerFOVMap = myFOV;
}

mapData::~mapData() {
    delete fileHandler;
    delete masterList;
}

void mapData::drawRoom(int x, int y, int w, int h) {
    int i, j;
    for (i=y;i<(y+h);i++) {
        for (j=x;j<(x+w);j++) {
            mapHandlerMaster[j][i].graphic = '.';
            mapHandlerMaster[j][i].isTransparent = true;
            mapHandlerMaster[j][i].isWalkable = true;
            mapHandlerMaster[j][i].foreColor = TCODColor::darkerGrey;
            if (i==y || i==(y+h-1)) {
                mapHandlerMaster[j][i].graphic = '#';
                mapHandlerMaster[j][i].foreColor = TCODColor::darkGrey;
                mapHandlerMaster[j][i].isTransparent = false;
                mapHandlerMaster[j][i].isWalkable = false;
            }
            if (j==x || j==(x+w-1)) {
                mapHandlerMaster[j][i].graphic = '#';
                mapHandlerMaster[j][i].foreColor = TCODColor::darkGrey;
                mapHandlerMaster[j][i].isTransparent = false;
                mapHandlerMaster[j][i].isWalkable = false;
            }
        }
    }
    // Now we cut a door into one side:
    for (i=(rand()%3+1);i>0;i--) {
        int side = (rand() % 4 + 1);
        cout<<"Side: "<<side<<endl;
        if (side == 1 || side == 2) {
            int door = (rand() % (w-2) + x+1);
            //cout<<"Door val: "<<door<<endl;
            if (side == 1) {
                mapHandlerMaster[door][y].graphic = '+';
                mapHandlerMaster[door][y].foreColor = TCODColor::copper;
                mapHandlerMaster[door][y].isWalkable = true;
                mapHandlerMaster[door][y].isTransparent = false;
            }
            if (side == 2) {
                mapHandlerMaster[door][y+h-1].graphic = '+';
                mapHandlerMaster[door][y+h-1].foreColor = TCODColor::copper;
                mapHandlerMaster[door][y+h-1].isWalkable = true;
                mapHandlerMaster[door][y+h-1].isTransparent = false;
            }
        } else {
            int door = (rand() % (h-2) + y+1);
            //cout<<"Door val: "<<door<<endl;
            if (side == 3) {
                mapHandlerMaster[x][door].graphic = '+';
                mapHandlerMaster[x][door].foreColor = TCODColor::copper;
                mapHandlerMaster[x][door].isWalkable = true;
                mapHandlerMaster[x][door].isTransparent = false;
            }
            if (side == 4) {
                mapHandlerMaster[x+w-1][door].graphic = '+';
                mapHandlerMaster[x+w-1][door].foreColor = TCODColor::copper;
                mapHandlerMaster[x+w-1][door].isWalkable = true;
                mapHandlerMaster[x+w-1][door].isTransparent = false;
            }
        }
    }
}

void mapData::genNewMap() {
    int newType = rand() % 2;
    if (newType == 0)
        myType = ice;
    else
        myType = ice;

    int i,j;
    for (i=0;i<SCREEN_WIDTH;i++) {
        for (j=0;j<SCREEN_HEIGHT;j++) {
            // Set the default char to whatever the fuck
            if (myType == ice) {
                int test = (rand() %10);
                if (test <= 3) {
                    mapHandlerMaster[i][j].graphic = '\'';
                    mapHandlerMaster[i][j].foreColor = TCODColor::white;
                }
                else if (test > 3 && test <=4) {
                    mapHandlerMaster[i][j].graphic = ',';
                    mapHandlerMaster[i][j].foreColor = TCODColor::lighterCyan;
                }
                else if (test == 5 || test == 6) {
                    int tempRoll = rand() % 3;
                    if (tempRoll == 0) {
                        mapHandlerMaster[i][j].graphic = '#';
                        mapHandlerMaster[i][j].foreColor = TCODColor::darkerGrey;
                    } else if (tempRoll == 1) {
                        mapHandlerMaster[i][j].graphic = ',';
                        mapHandlerMaster[i][j].foreColor = TCODColor::darkGrey;
                    } else if (tempRoll == 2) {
                        mapHandlerMaster[i][j].graphic = ';';
                        mapHandlerMaster[i][j].foreColor = TCODColor::lighterCyan;
                    }
                }
                else if (test >= 7) {
                    mapHandlerMaster[i][j].graphic = '.';
                    mapHandlerMaster[i][j].foreColor = TCODColor::grey;
                }
            } else if (myType == barren) {
                int test = (rand() % 4);
                if (test <= 1) {
                    mapHandlerMaster[i][j].graphic = '=';
                    mapHandlerMaster[i][j].foreColor = TCODColor::darkerGrey;
                }
                else if (test == 2) {
                    mapHandlerMaster[i][j].graphic = '-';
                    mapHandlerMaster[i][j].foreColor = TCODColor::lightCyan;
                }
                else if (test == 3) {
                    mapHandlerMaster[i][j].graphic = '_';
                    mapHandlerMaster[i][j].foreColor = TCODColor::lightCyan;
                }
            }
        }
    }
    /*if (mpShort->mapHandlerMaster[i][j] == '#')
        offscreenConsole->setFore(i, j, TCODColor::lightBlue);
    if (mpShort->mapHandlerMaster[i][j] == '|')
        offscreenConsole->setFore(i, j, TCODColor::darkSepia);
    if (mpShort->mapHandlerMaster[i][j] == '=')
        offscreenConsole->setFore(i, j, TCODColor::darkerGrey);
    if (mpShort->mapHandlerMaster[i][j] == '?')
        offscreenConsole->setFore(i, j, TCODColor::black);
    if (mpShort->mapHandlerMaster[i][j] == '^')
        offscreenConsole->setFore(i, j, TCODColor::lightestBlue);*/
    int fullbladder = rand() % 10 + 4;
    drawRoom(rand() % 20 + 1, rand() % 20 + 1, 10, 10);
    for (i=0;i<fullbladder;i++) {
        // Generates a test entity:
        entity *test = masterList->insert(ENEMY);
        if (test == NULL) {
            exit(1);
        }
        test->myStats->level = (rand() % mapLevel + (mapLevel-1));
        test->randEnt(playerFOVMap);
        test->pos[0] = (rand() % 75 +3);
        test->pos[1] = (rand() % 75 +3);
    }

    loaded = true;
}

void mapData::noise(noiseType newNoise, TCODConsole *myDest, cPlayer *mainPlayer) {
    if (newNoise == snow) {
        int test = mainPlayer->snowLevel;
        for (int i=0;i<test;i++) {
            int noisePos[2] = { rand()%80, rand()%50 };

            if (mainPlayer->myFOV->isInFov(noisePos[0], noisePos[1])) {
                float distance = (float)((noisePos[0] - mainPlayer->pos[0]) * (noisePos[0] - mainPlayer->pos[0]) + 
                    (noisePos[1] - mainPlayer->pos[1]) * (noisePos[0] - mainPlayer->pos[1]));
                float squaredTorchRadius = (float)(mainPlayer->lightRadius * mainPlayer->lightRadius);

			    if(distance < squaredTorchRadius) {
                    float lerpVal = (squaredTorchRadius - distance) / squaredTorchRadius;
                    lerpVal = CLAMP(0.1f, 1.0f, lerpVal);
                    TCODColor foreColor = TCODColor::lighterCyan * mainPlayer->lightColor * lerpVal;
                    myDest->setFore(noisePos[0], noisePos[1], foreColor);
                    int snowChar = rand() % 3;
                    switch(snowChar) {
                    case 0:
                        myDest->setChar(noisePos[0], noisePos[1], '"');
                        break;
                    case 1:
                        myDest->setChar(noisePos[0], noisePos[1], '+');
                        break;
                    case 2:
                        myDest->setChar(noisePos[0], noisePos[1], '%');
                        break;
                    }
                }
            }
        }
    }
}

void mapData::clearMap() {
    int i, j;
    masterList->removeAll();
    for (i=0;i<SCREEN_HEIGHT;i++) {
        for (j=0;j<SCREEN_WIDTH;j++) {
            mapHandlerMaster[j][i].isExplored = false;
        }
    }
}

void mapData::randChanges(cPlayer *mainPlayer) {
    int randEvent = rand() % 4;
    switch (randEvent) {
    case 0:
        mainPlayer->newMessage("The night is cold and silent.", TCODColor::lightBlue);
        mainPlayer->lightRadius = 10;
        mainPlayer->isSnowing = false;
        break;
    case 1:
        mainPlayer->newMessage("It has begun to snow lightly.", TCODColor::lightBlue);
        mainPlayer->lightRadius = 8;
        mainPlayer->isSnowing = true;
        mainPlayer->snowLevel = 125;
        break;
    case 2:
        mainPlayer->newMessage("A snowstorm has arrived.", TCODColor::lightBlue);
        mainPlayer->lightRadius = 6;
        mainPlayer->isSnowing = true;
        mainPlayer->snowLevel = 200;
        break;
    case 3:
        mainPlayer->newMessage("You are trapped in a blizzard!", TCODColor::lightBlue);
        mainPlayer->lightRadius = 5;
        mainPlayer->isSnowing = true;
        mainPlayer->snowLevel = 500;
        break;
    }
    
}