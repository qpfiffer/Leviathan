#include "map.h"
#include "main.h"
#include "entities.h"
#include "file_handler.h"

#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3

// CONSTRUCTORS AND DESTRUCTORS

cPlayer::cPlayer() {
    // Important stuff?
    myName = "WALLY";
    myColor = TCODColor::blue;
    int i;
    for (i=0;i<2;i++) {
        pos[i] = 25;
        cell[i] = 0;
    }

    // Map stuff
    collides = "#v|^/o";
    snowLevel = 200;
    isSnowing = false;
    turns = 0;
    nextRand = rand() % 300;

    // Light/FoV stuff
    lightColor.r=255;
    lightColor.g=255;
    lightColor.b=255;
    lightRadius = 10;
    myFOV = new TCODMap(80, 50);
    mapDataHandle = new mapData(myFOV);

    // Information
    messagesHandle[0] = " ";
    messagesHandle[1] = " ";
    messagesHandle[2] = " ";
    messageColors[0] = TCODColor::white;
    messageColors[1] = TCODColor::white;
    messageColors[2] = TCODColor::white;

    // Character creation:
    myLevel = 1;
    xp = 0;
    str = rand() % 5 + 1;
    con = rand() % 5 + 1;
    health = 20 + (rand() % 10 + 1);
    sleepReq = 1;


}

cPlayer::~cPlayer() {
    delete mapDataHandle;
    delete myFOV;
}

stats::stats()
{
    str=1;
	con=1;
	level=1;
    //intel=1;
	//foc=1;
	//dex=1;

	//science=1;
	//guns=1;
	//fisticuffs=1;
	//sticks=1;
	//driving=1;
	
	health=25;
	/*mind=(intel*10)+(foc*10);
	body=(str*10)+(con*10);*/

	/*dodgeValue = rand()%3+1;*/
	sleepReq = 1;
}

entity::entity() {
    int i;
    for (i=0;i<2;i++) {
        pos[i] = 0;
        size[i] = 1; //1x1 for default
    }

    myName = "HUNGRAGAR";
    myEntType = ITEM;
    myStats = NULL;
    myChar = 'D';
    myColor = TCODColor::blue;

    next = NULL;
}

entity::entity(entType newType) {
    int i;
    for (i=0;i<2;i++) {
        pos[i] = 0;
        size[i] = 1; //1x1 for default
    }

    if (newType != ITEM) {
        myStats = new stats;
    }

    myEntType = newType;
    myName = "SALAZAR";
    myChar = 'A';
    myColor = TCODColor::red;

    next = NULL;
}

entity::~entity() {
    if (myEntType != ITEM) {
        delete myStats;
        if (myPath)
            delete myPath;
    }
}

entityList::entityList() {
    size=0;
    head = NULL;
    names = new TCODNamegen;
    names->parse("data/inuit.txt");
}

entityList::~entityList() {
    names->destroy();
    delete names;
}

// ---
// END CONSTRUCTORS/DESTRUCTORS
// ---

void cPlayer::updateFOV() {
    int i, j;
    for (i=0;i<50;i++) {
        for (j=0;j<80;j++) {
            int checkPos[2] = { j, i };
            //checkStaticCollides(checkPos)
            if (mapDataHandle->mapHandlerMaster[j][i].graphic == '+') {
                myFOV->setProperties(j, i, false, true);
            } else {
                myFOV->setProperties(j, i, checkStaticCollides(checkPos), checkStaticCollides(checkPos));
            }
            
        }
    }
    myFOV->computeFov(pos[0], pos[1], lightRadius, true, FOV_BASIC);
}

void entity::colCallback(cPlayer *mainPlayer) {
    // If this function is called, it means the mainPlayer
    // Bumped into us. Put attack code/whatever here.

    // Lets just assume that we're some item that is getting
    // Picked up for now:
    if (myEntType == ITEM) {
        std::cout<<mainPlayer->myName<<" picked up a "<<myName<<"!"<<std::endl;
        mainPlayer->mapDataHandle->masterList->remove(this);
    } 
    if (myEntType == ENEMY) {
        int toHit = rand() % 5 + 1;
        if (toHit >= 4) {
            int damage = rand() % (myStats->str) + 1;
            std::ostringstream text;
            //std::cout<<"DAMAGE: "<<damage<<std::endl;

            text << "You hit "<<myName<<" for " << damage
                 << " damage. ";

            mainPlayer->newMessage(text.str(), TCODColor::cyan);
            myStats->health-=damage;
        } else {
            mainPlayer->newMessage("You fail to hit "+myName, TCODColor::yellow);
        }
        if (myStats->health <= 0) {
            mainPlayer->newMessage("You killed "+myName+"!", TCODColor::cyan);
            mainPlayer->xp += rand() % 5 + myStats->level;
            mainPlayer->mapDataHandle->masterList->remove(this);
        }
    }
    if (myEntType == FRIEND) {
        mainPlayer->newMessage(myName+" is your friend. You cannot attack.", TCODColor::cyan);
        //mainPlayer->mapDataHandle->masterList->remove(this);
    }
}

entity *cPlayer::checkEntCollides(int direction) {
    entity *loop = this->mapDataHandle->masterList->head;
    while (loop != NULL) {
        switch(direction) {
        case(LEFT):
            if (loop->pos[0] == (pos[0]-1) &&
                loop->pos[1] == pos[1]) {
                return loop;
            }
            break;
        case(RIGHT):
            if (loop->pos[0] == (pos[0]+1) &&
                loop->pos[1] == pos[1]) {
                return loop;
            }
            break;
        case(UP):
            if (loop->pos[1] == (pos[1]-1) &&
                loop->pos[0] == pos[0]) {
                return loop;
            }
            break;
        case(DOWN):
            if (loop->pos[1] == (pos[1]+1) &&
                loop->pos[0] == pos[0]) {
                return loop;
            }
            break;
        }
        loop = loop->next;
    }
    return 0;
}

void entity::randEnt(TCODMap *newMap)
{
    if (myEntType == ITEM)
        return;
    //Stats
    myStats->str = (rand() % (5 + myStats->level)) + 1;
    myStats->con = (rand() % (5 + myStats->level)) + 1;
    /*myStats->intel = rand() % 5 + 1;
    myStats->foc = rand() % 5 + 1;
    myStats->dex = rand() % 5 + 1;*/
    //Skills
    /*myStats->science = rand() % 5 + 1;
    myStats->guns = rand() % 5 + 1;
    myStats->fisticuffs = rand() % 5 + 1;
    myStats->sticks = rand() % 5 + 1;
    myStats->driving = rand() % 5 + 1;*/
    //Health, Mind, and Body
    myStats->health = myStats->con + 20;
    if (myStats->con > 5)
        myStats->health = 120;

    /*myStats->mind = (myStats->intel * 10) + (myStats->foc * 10);
    myStats->body = (myStats->str * 10) + (myStats->con * 10);

    myStats->dodgeValue = myStats->dex + myStats->foc + ((rand() + 1) % 5);*/

    char temp[10] = {'A','B','E','?','J','C','M','8','$','X'};
    myChar = temp[rand()%10];
    myColor.r = rand() % 255;
    myColor.g = rand() % 255;
    myColor.b = rand() % 255;

    myPath = new TCODPath(newMap, 2.5f);

}

void entity::attack(cPlayer *mainPlayer) {
    int toHit = rand() % 5 + 1;
    if (toHit >= 4) {
        int damage = rand() % (myStats->str) + 1;
        std::ostringstream text;
        std::cout<<"DAMAGE: "<<damage<<std::endl;

        text <<  myName << " hits you for " << damage
             << " damage. " << mainPlayer->health << " health left.";

        mainPlayer->newMessage(text.str(), TCODColor::red);
        mainPlayer->health-=damage;
    } else {
        mainPlayer->newMessage(myName+" fails to hit you.", TCODColor::yellow);
    } 
}

void cPlayer::newMessage(std::string newMess, TCODColor color) {
    messagesHandle[2] = messagesHandle[1];
    messagesHandle[1] = messagesHandle[0];
    messagesHandle[0] = newMess;
    messageColors[2] = messageColors[1];
    messageColors[1] = messageColors[0];
    messageColors[0] = color;
}

bool cPlayer::checkStaticCollides(int dest[2]) {
    char test = mapDataHandle->mapHandlerMaster[dest[0]][dest[1]].graphic;
    int i;
    for (i=0;i<sizeof(collides);i++) {
        if (test == collides[i])
            return false;
    }
    return true;
}

entity *entityList::insert(entType newType) {
    entity *newEntity = new entity(newType);

    if (head == NULL) {
        head = newEntity;
        newEntity->next = NULL;
        return newEntity;
    } else { 
        entity *nextEntity = head;
        while(nextEntity != NULL) {
            // If we are at the end of the list:
            if (nextEntity->next == NULL) {
                // Make the old end point to the new end:
                nextEntity->next = newEntity;
                // Make sure the new end is NULL
                newEntity->next = NULL;
                newEntity->myName = names->generate("inuit");
                return newEntity;
            } else {
                nextEntity = nextEntity->next;
            }
        }
    }
    return 0;

}

void entityList::remove(entity *removeMe) {
    entity *prev = NULL;
    entity *current = head;

    entity *old;
    while (current != NULL) {
        if (current == removeMe) {
            if (prev == NULL) {
                // If previous is NULL, we're removing the head 
                // of the linked list.
                old = head;
                head = head->next;
                delete old;
                return;
            } else {
                prev->next = current->next;
                delete current;
                return;
            }
        } else {
            prev = current;
            current = current->next;
        }
    }
}

void entityList::removeAll() {
    entity *cur = head;
    entity *next = NULL;
    if (head != NULL)
        next = head->next;
    else
        return; // This list is empty.

    while (cur != NULL) {
        next = cur->next;
        delete cur;
        cur = NULL;
        cur = next;
    }
    head = NULL;
}
