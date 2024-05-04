#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_

//=================================================================
//Data to be passed by game to all Game Objects via Tick
//=================================================================

#include "GameState.h"
#include "Keyboard.h"
#include "Mouse.h"
//#include "Game.h"

using namespace DirectX;

struct GameData
{
	float m_dt;  //time step since last frame
	GameState m_GS; //global GameState
	bool cursorShowing = false;

	//player input
	Keyboard::State m_KBS;
	Mouse::State m_MS;
	Keyboard::KeyboardStateTracker m_KBS_tracker;
	//Game game;
	bool m_hand_anim, gravity_on = true, m_can_jump = true;

	//void FireProj() { game.FireProjectile(); };

};
#endif
