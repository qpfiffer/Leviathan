#pragma once
#include <string>

enum entType {FRIEND, ENEMY, ITEM};
// Forward declarations (as needed):
class cPlayer;
class mapData;

class stats
{
public:
	stats();
	//basic stats and stuff
	int str;
    int level;
	int con;
	//int intel;
	//int foc;
	//int dex;
	//these are the skills
	//int science;
	//int guns;
	//int fisticuffs;
	//int sticks;
	//int driving;
	//health, mind, body
	int health;
	/*int mind;
	int body;*/
    float sleepTerror;
	//hidden stats
	//int dodgeValue;
    int sleepReq;
};

class entity {
public:
    entity();
    entity(entType newType);
    ~entity();

    void colCallback(cPlayer *mainPlayer);
    void attack(cPlayer *mainPlayer);
    void randEnt(TCODMap *newMap);

    std::string myName;
    stats *myStats;
    entType myEntType;
    int pos[2];
    int size[2]; // Like if you wanted cars or something (2x2 bricks)
    char myChar;
    TCODColor myColor;

    TCODPath *myPath;

    entity *next;
};

class cPlayer : public stats {
public:
    cPlayer();
    ~cPlayer();

    entity *checkEntCollides(int direction);
    bool checkStaticCollides(int dest[2]);
    void updateFOV();
    void newMessage(std::string newMess, TCODColor color);

    std::string myName;
    std::string messagesHandle[3];
    TCODColor messageColors[3];

    TCODColor myColor;
    TCODColor lightColor;
    TCODMap *myFOV;
    mapData *mapDataHandle;

    char *collides;
    bool isSnowing;
    int lightRadius;
    int snowLevel;
    int turns;
    int nextRand;
    int pos[2];
    int cell[2];
    int myLevel;
    int xp;
};

class entityList {
public:
    entityList();
    ~entityList();

    entity *head;
    TCODNamegen *names;

    int size;

    // Returns a pointer to a fresh entity
    entity *insert(entType newType);
    // Pass this function an entity
    void remove(entity *removeMe);
    void removeAll();
};