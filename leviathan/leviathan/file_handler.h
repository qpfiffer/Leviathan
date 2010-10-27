#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
using namespace std;

// Forward declares:
class mapData;

class cellFile {
public:
    cellFile();
    cellFile(int cellX, int cellY);
    ~cellFile();

    int readMapData(mapData *mainData);
    int writeMapData(mapData *mainData);

    string myName;
    int mCellNum[2];

    int numEnt;

    ifstream cellIn;
    ofstream cellOut;
};

class configInfo {
public:
    configInfo();      // Constructor (duh)
    configInfo(int x, int y);
    int readConfig();  // Reads configuration file
    void printVars();  // Prints out vars for debuggings
private:
    char *lMapHandler;
    string readTarget;
    ifstream config;   // Config file fstream handler
};

class logFile {
public:
    logFile();

    int readLogFile();
    int closeLogfile();

    ofstream log;
};
#endif
