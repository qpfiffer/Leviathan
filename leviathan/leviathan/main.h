#pragma once
// Custom Libs:
#include <libtcod.hpp>
// C++ Libs:
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
// C Libs:
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 50

class cPlayer;

// GLOBAL FUNCS (Why so C?)
void render(cPlayer *mainPlayer, TCODConsole *messages);

int keyHandler(cPlayer *mainPlayer);