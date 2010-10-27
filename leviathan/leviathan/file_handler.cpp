#include "map.h"
#include "main.h"
#include "entities.h"
#include "file_handler.h"

//CONSTRUCTORS
configInfo::configInfo() {
}

logFile::logFile() {   
}
//PUBLIC METHODS

// CONFIG FILE

int configInfo::readConfig() {
    config.open(readTarget, ifstream::in);
    char temp[25];
    while (config.good()) { // EOF, badbit and failbit
        config.getline(temp, 24, '=');
        //if (strcmp(temp, "width") == 0) {
        //    config.getline(temp, 10, '\n');
        //    width=atoi(temp);
        //}
    }
    config.close();

    return 0;
}

void configInfo::printVars() {
}

// LOG FILE
int logFile::readLogFile() {
    log.open("lab2.log", ifstream::out);
    return 0;
}

int logFile::closeLogfile() {
    if (log.good())
        log.close();
    return 0;
}

// CELL FILE
cellFile::cellFile() {
    myName = "data/cell_0_0.map";
    for(int i=0;i<2;i++)
        mCellNum[i] = 0;

    numEnt = 0;
}

cellFile::cellFile(int cellX, int cellY) {
    // I KNOW HOW CONSTRUCTORS WORK
    numEnt = 0;
    mCellNum[0] = cellX;
    mCellNum[1] = cellY;
    myName+="data/cell_"+(int)cellX;
    myName+="_"+(int)cellY;
    myName+=".map";
}

int cellFile::readMapData(mapData *mainData) {
    char blank[81];
    int i=0, j=0;

    cellIn.open(myName.c_str(), fstream::in );
    if (cellIn.fail()) {
        // If we failed to open the cell, that means we
        // are making a new one.
        cellOut.open(myName.c_str(), fstream::out);
        cellOut<<"BLANK"<<endl;
        cellOut.close();
        cellOut.clear();

        return 1;
    }

    while (cellIn.good()) { // EOF, badbit and failbit
        // Read in characters until we get to the end of the line
        cellIn.unsetf(ios_base::skipws);
        cellIn.width(6);
        cellIn>>blank;
        if (strcmp(blank, "BLANK")==0) {
            // HURR DURR I AM BAD AT BOOLEAN
            //cout<<"Strcmp is: "<<strcmp(blank, "BLANK")<<endl;
            cout<<"First 5 chars of my cell: "<<blank<<endl;
            cout<<"This cell is blank."<<endl;
            return 1;
        }
        cout<<"First 5 chars of my cell: "<<blank<<endl;
        cellIn.seekg (0, ios::beg);
            
        for (i=0;i<50;i++) {
            cellIn.width(81);
            cellIn>>blank;
            if (cellIn.good()) {
                for (j=0;j<80;j++) {
                    mainData->mapHandlerMaster[j][i].graphic = blank[j];
                    //cout<<blank[j];
                }
                //system("PAUSE");
                //cout<<endl;
            } else {
                cout<<"Bits were tripped."<<endl;
                if (cellIn.rdstate() == istream::eofbit)
                    cout<<"EOF"<<endl;
                if (cellIn.rdstate() == istream::failbit)
                    cout<<"Failbit"<<endl;
                if (cellIn.rdstate() == istream::badbit)
                    cout<<"Badbit"<<endl;
                return 1;
            }
        }
        break;
        // Getline automagically moves up a line, our array
        // has to be pushed into it, hence the use of j.
    }

    cellIn.close();
    cellIn.clear();

    mainData->loaded=true;
    return 0;
}

int cellFile::writeMapData(mapData *mainData) {
    int i=0, j=0;

    if (!cellOut.is_open())
        cellOut.open(myName.c_str(), fstream::out);

    if (cellOut.fail())
        return 1;

    while (cellOut.good()) { // EOF, badbit and failbit
        // Write the map array
        for (i=0;i<50;i++) {
            for (j=0;j<80;j++) {
                cellOut<<mainData->mapHandlerMaster[j][i].graphic;
            }
        }
        break;
        // Getline automagically moves up a line, our array
        // has to be pushed into it, hence the use of j.
    }

    cellOut.close();
    cellOut.clear();

    return 0;
}

cellFile::~cellFile() {
    if (cellIn.is_open())
        cellIn.close();
    cellIn.clear();
    if (cellOut.is_open())
        cellOut.close();
    cellOut.clear();
}