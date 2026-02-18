/**
 * @file main.cpp
 * @brief Entry point for UNO-Lite.
 * @author Tuan
 */

#include "Game.h"
#include <cstdlib>
#include <ctime>

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;
    game.setupGame();
    game.gameLoop();

    return 0;
}
