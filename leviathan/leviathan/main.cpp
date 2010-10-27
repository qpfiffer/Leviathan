#include "map.h"
#include "main.h"
#include "entities.h"
#include "file_handler.h"
using namespace std;

// Don't change these. My code isn't as modular as it looks.
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 50

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

void render(cPlayer *mainPlayer, TCODConsole *messages) {
    int i, j;
    // Shorten this shit up a bit:
    mapData *mpShort = mainPlayer->mapDataHandle;

    TCODConsole *offscreenConsole = new TCODConsole(SCREEN_WIDTH,SCREEN_HEIGHT);
    offscreenConsole->setForegroundColor(TCODColor::blue);
    offscreenConsole->clear(); // Flush the new changes to the offscreen console

    // Check if we have loaded the current cell yet:
    if (mpShort->loaded != true) {
        //if(mpShort->fileHandler->readMapData(mpShort) == 1) {
        //    cout<<"Failed to load map data."<<endl;
            mpShort->clearMap();
            mpShort->genNewMap();
        //    mpShort->fileHandler->writeMapData(mpShort);
        //    cout<<"Map generated and written."<<endl;
        //} else {
        //    cout<<"Map loaded."<<endl;
        //}
    }

    mainPlayer->updateFOV();
    // Map renderer:
    for (i=0;i<SCREEN_WIDTH;i++) {
        for (j=0;j<SCREEN_HEIGHT;j++) {
            if (mainPlayer->myFOV->isInFov(i, j)) {
                // If it is in the Field of View, make sure it has been explored now:
                mpShort->mapHandlerMaster[i][j].isExplored = true;

                float distance = (float)((i - mainPlayer->pos[0]) * (i - mainPlayer->pos[0]) + 
                    (j - mainPlayer->pos[1]) * (j - mainPlayer->pos[1]));
                float squaredTorchRadius = (float)(mainPlayer->lightRadius * mainPlayer->lightRadius);

				if(distance < squaredTorchRadius) {
                    float lerpVal = (squaredTorchRadius - distance) / squaredTorchRadius;
                    lerpVal = CLAMP(0.1f, 1.0f, lerpVal);
                    TCODColor foreColor = mpShort->mapHandlerMaster[i][j].foreColor * mainPlayer->lightColor * lerpVal;
					TCODColor backColor = mpShort->mapHandlerMaster[i][j].backColor * mainPlayer->lightColor * lerpVal;
                    offscreenConsole->setFore(i, j, foreColor);
                    offscreenConsole->setBack(i, j, backColor);
                    offscreenConsole->setChar(i, j, mpShort->mapHandlerMaster[i][j].graphic);
                }
            } else if(mpShort->mapHandlerMaster[i][j].isExplored) {	// If the tile is not in fov, but has been seen before...
				offscreenConsole->setFore(i, j, mpShort->mapHandlerMaster[i][j].foreColor * 0.1f);
                offscreenConsole->setBack(i, j, mpShort->mapHandlerMaster[i][j].backColor * 0.1f);
                offscreenConsole->setChar(i, j, mpShort->mapHandlerMaster[i][j].graphic);
			} else {	// And if the tile has never been seen, we just black it out.
				offscreenConsole->putCharEx(i, j, ' ', TCODColor::black, TCODColor::black);
            }
        }
    }

    // Entity renderer:
    entity *loop = mainPlayer->mapDataHandle->masterList->head;
    while (loop != NULL) {
        // We have entities to render:
        if (mainPlayer->myFOV->isInFov(loop->pos[0], loop->pos[1])) {
            float distance = (float)((loop->pos[0] - mainPlayer->pos[0]) * (loop->pos[0] - mainPlayer->pos[0]) + 
                    (loop->pos[1] - mainPlayer->pos[1]) * (loop->pos[1] - mainPlayer->pos[1]));
            float squaredTorchRadius = (float)(mainPlayer->lightRadius * mainPlayer->lightRadius);

			if(distance < squaredTorchRadius) {
                float lerpVal = (squaredTorchRadius - distance) / squaredTorchRadius;
                lerpVal = CLAMP(0.1f, 1.0f, lerpVal);
                TCODColor foreColor = loop->myColor * mainPlayer->lightColor * lerpVal;
                offscreenConsole->setFore(loop->pos[0], loop->pos[1], foreColor);
                offscreenConsole->setChar(loop->pos[0], loop->pos[1], loop->myChar);

                // Compute the path stuff for the entity:
                loop->myPath->compute(loop->pos[0], loop->pos[1], mainPlayer->pos[0], mainPlayer->pos[1]);
                // See if we are one space away from the player, and attack him if we are:
                int qX=0, qY=0;
                loop->myPath->walk(&qX, &qY, true);
                if (qX == mainPlayer->pos[0] && qY == mainPlayer->pos[1]) {
                    loop->attack(mainPlayer);
                } else {
                    loop->pos[0] = qX;
                    loop->pos[1] = qY;
                }
            }
        }
        
        loop = loop->next;
    }

    // Some random event stuff:
    if (mainPlayer->turns == mainPlayer->nextRand) {
        mainPlayer->mapDataHandle->randChanges(mainPlayer);
        mainPlayer->nextRand = mainPlayer->turns+=rand() % 300;
    }

    // Noise stuff:
    if (mainPlayer->isSnowing)
        mpShort->noise(snow, offscreenConsole, mainPlayer);
    
    // Render the player last
    offscreenConsole->setFore(mainPlayer->pos[0], mainPlayer->pos[1], mainPlayer->myColor);
    offscreenConsole->setChar(mainPlayer->pos[0], mainPlayer->pos[1], '@');

    // Blit all that crap on-screen
    TCODConsole::blit(offscreenConsole,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,TCODConsole::root,0,0, 1.0f, 1.0f);

    // Messages:
    // clear and remake the window:
    for (i=1;i<4;i++) {
        for (j=1;j<(SCREEN_WIDTH-1);j++) {
            messages->setChar(j, i, ' ');
        }
    }

    for (i=0;i<3;i++) {
        std::ostringstream temp;
        temp <<"%c"<<mainPlayer->messagesHandle[i]<<"%c";
        TCODConsole::setColorControl(TCOD_COLCTRL_1,mainPlayer->messageColors[i],TCODColor::black);
        messages->printEx(1, (i+1), TCOD_BKGND_NONE, TCOD_LEFT, temp.str().c_str(),TCOD_COLCTRL_1,TCOD_COLCTRL_STOP);
    }
    TCODConsole::blit(messages, 0, 0, SCREEN_WIDTH, 5, TCODConsole::root, 0, SCREEN_HEIGHT);
    
    // Flip it
    TCODConsole::root->flush();

    delete offscreenConsole;
}

int keyHandler(cPlayer *mainPlayer) {
    int keepGoing = 1;
    while (keepGoing == 1) {
        TCOD_key_t key = TCODConsole::waitForKeypress(true);
        mainPlayer->turns++;
        if (mainPlayer->turns % 1000 == 0) {
            mainPlayer->sleepReq++;
            mainPlayer->newMessage("You feel more tired.", TCODColor::yellow);
        }
    
        // Movement:
        int moveflags = -1;

        switch(key.vk) {
        case(TCODK_ESCAPE):
            return -1;
            break;
        case(TCODK_LEFT):
            moveflags = LEFT;
            break;
        case(TCODK_RIGHT):
            moveflags = RIGHT;
            break;
        case(TCODK_UP):
            moveflags = UP;
            break;
        case(TCODK_DOWN):
            moveflags = DOWN;
            break;
        }
        if (key.c == 'd')
            cout<<"X: "<<mainPlayer->pos[0]<<" Y: "<<mainPlayer->pos[1]<<endl;
        if (key.c == '5')
            keepGoing = 0;
        if (key.c == 'f') {
            TCODConsole::setFullscreen(!TCODConsole::isFullscreen());
            keepGoing = 0;
        }

        // Check if our movement flag has been tripped
        if (moveflags != -1) {
            entity *entColCheck = NULL;

            int dest[2] = {mainPlayer->pos[0],mainPlayer->pos[1]};
            switch(moveflags) {
            case LEFT:
                dest[0] = (mainPlayer->pos[0])-1;
                break;
            case RIGHT:
                dest[0] = (mainPlayer->pos[0])+1;
                break;
            case UP:
                dest[1] = (mainPlayer->pos[1])-1;
                break;
            case DOWN:
                dest[1] = (mainPlayer->pos[1])+1;
                break;
            }
            //cout<<"DESTINATION: X: "<<dest[0]<<" Y: "<<dest[1]<<endl;

            // Check to see if we're moving into a new cell:
            if (dest[0] == SCREEN_WIDTH) {
                mainPlayer->mapDataHandle->loaded = false;
                //shortP->mCellNum[0]++;
                mainPlayer->pos[0] = 0;
                //cellFile *tempP = new cellFile();
                //mainPlayer->mapDataHandle->fileHandler = tempP;
                //delete shortP;
                return 0;
            }

            if (dest[0] < 0) {
                mainPlayer->mapDataHandle->loaded = false;
                //shortP->mCellNum[0]++;
                mainPlayer->pos[0] = SCREEN_WIDTH-1;
                //cellFile *tempP = new cellFile();
                //mainPlayer->mapDataHandle->fileHandler = tempP;
                //delete shortP;
                return 0;
            }

            if (dest[1] < 0) {
                mainPlayer->mapDataHandle->loaded = false;
                //shortP->mCellNum[0]--;
                mainPlayer->pos[1] = (SCREEN_HEIGHT-1);
                //cellFile *tempP = new cellFile();
                //mainPlayer->mapDataHandle->fileHandler = tempP;
                //delete shortP;
                return 0;
            }

            if (dest[1] == SCREEN_HEIGHT) {
                mainPlayer->mapDataHandle->loaded = false;
                //shortP->mCellNum[0]++;
                mainPlayer->pos[1] = 0;
                //cellFile *tempP = new cellFile();
                //mainPlayer->mapDataHandle->fileHandler = tempP;
                //delete shortP;
                return 0;
            }


            switch(moveflags) {
            case(LEFT):
                if ((entColCheck = mainPlayer->checkEntCollides(LEFT)) == 0 &&
                    mainPlayer->checkStaticCollides(dest) == true) {
                    mainPlayer->pos[0]--;
                } else {
                    if (entColCheck != NULL) {
                        // Look like we hit an entity.
                        entColCheck->colCallback(mainPlayer);
                    }
                    //cout<<"Collision!"<<endl;
                }
                keepGoing = 0;
                break;
            case(RIGHT):
                if ((entColCheck = mainPlayer->checkEntCollides(RIGHT)) == 0 &&
                    mainPlayer->checkStaticCollides(dest) == true) {
                    mainPlayer->pos[0]++;
                } else {
                    if (entColCheck != NULL) {
                        // Look like we hit an entity.
                        entColCheck->colCallback(mainPlayer);
                    }
                    //cout<<"Collision!"<<endl;
                }
                keepGoing = 0;
                break;
            case(UP):
                if ((entColCheck = mainPlayer->checkEntCollides(UP)) == 0 &&
                    mainPlayer->checkStaticCollides(dest) == true) {
                    mainPlayer->pos[1]--;
                } else {
                    if (entColCheck != NULL) {
                        // Look like we hit an entity.
                        entColCheck->colCallback(mainPlayer);
                    }
                    //cout<<"Collision!"<<endl;
                }
                keepGoing = 0;
                break;
            case(DOWN):
                if ((entColCheck = mainPlayer->checkEntCollides(DOWN)) == 0 &&
                    mainPlayer->checkStaticCollides(dest) == true) {
                    mainPlayer->pos[1]++;
                } else {
                    if (entColCheck != NULL) {
                        // Look like we hit an entity.
                        entColCheck->colCallback(mainPlayer);
                    }
                    //cout<<"Collision!"<<endl;
                }
                keepGoing = 0;
                break;
            }
        }
    }
    return 0;
}
int main() {
    srand((int)time(NULL));
    cPlayer *mainPlayer = new cPlayer;

    TCODConsole::initRoot(SCREEN_WIDTH, SCREEN_HEIGHT+5, "LEVIATHAN", false, TCOD_RENDERER_SDL);

    mapData *mpShort = mainPlayer->mapDataHandle;
    if (mpShort->loaded != true) {
        if(mpShort->fileHandler->readMapData(mpShort) == 1) {
            cout<<"Failed to load map data."<<endl;
            mpShort->genNewMap();
            mpShort->fileHandler->writeMapData(mpShort);
            cout<<"Map generated and written."<<endl;
        } else {
            cout<<"Map loaded."<<endl;
        }
    }
    mpShort->genNewMap();

    TCODConsole *messages = new TCODConsole(SCREEN_WIDTH, 5);
    messages->setForegroundColor(TCODColor::red);
    messages->clear();
    int i,j;
    for (i=0;i<5;i++) {
        for (j=0;j<SCREEN_WIDTH;j++) {
            if (i==0) {
                messages->setFore(j, i, TCODColor::white);
                messages->setChar(j, i, '-');
            }
            if (i==4) {
                messages->setFore(j, i, TCODColor::white);
                messages->setChar(j, i, '-');
            }
            if (j==0) {
                messages->setFore(j, i, TCODColor::white);
                messages->setChar(j, i, '|');
            }
            if (j==SCREEN_WIDTH-1) {
                messages->setFore(j, i, TCODColor::white);
                messages->setChar(j, i, '|');
            }
            if (i==0 && (j==0 || j==SCREEN_WIDTH-1)) {
                messages->setFore(j, i, TCODColor::white);
                messages->setChar(j, i, '0');
            }
            if (i==4 && (j==0 || j==SCREEN_WIDTH-1)) {
                messages->setFore(j, i, TCODColor::white);
                messages->setChar(j, i, '0');
            }
        }
    }

    // Render once before everything, because we handle
    // keypresses first
    render(mainPlayer, messages);

    // Main loop
    while(1) {
        // See if we should be closed
        if (TCODConsole::isWindowClosed())
            break;

        if (mainPlayer->health <= 0) {
            // Losing condition:
            TCODConsole::root->setForegroundColor(TCODColor::white);
            TCODConsole::root->clear();
            /*for (int fade=255;fade >= 0;fade--) {
                TCODConsole::setFade(fade,TCODColor::black);
                TCODConsole::flush();
                Sleep(200);
            }*/
            string end = "You are dead.";
            TCODConsole::root->printEx(SCREEN_WIDTH/2-(sizeof("You are dead.")/2),
                SCREEN_HEIGHT/2, TCOD_BKGND_NONE, TCOD_LEFT, end.c_str());
            TCODConsole::flush();

            TCODConsole::waitForKeypress(true);
            break;
        } else if (keyHandler(mainPlayer) == -1) {
            break;
        }

        // Render everything
        render(mainPlayer, messages);
    }

    delete mainPlayer;
    delete messages;
    return 0;
}