#include "game.hpp"
#include "WumpusEngine.hpp"

WumpusEngine GameEngine;

///////////////////////////////////////////////////////////////////////////
//
// init()
//
// setup your game here
//
void init()
{
    GameEngine.Start();
}

///////////////////////////////////////////////////////////////////////////
//
// render(time)
//
// This function is called to perform rendering of the game. time is the 
// amount if milliseconds elapsed since the start of your game
//
void render(uint32_t time)
{
    GameEngine.Render();
}

///////////////////////////////////////////////////////////////////////////
//
// update(time)
//
// This is called to update your game state. time is the 
// amount if milliseconds elapsed since the start of your game
//
void update(uint32_t time)
{
    GameEngine.Update();
}