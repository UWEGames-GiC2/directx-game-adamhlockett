#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "CMOGO.h" 
#include <iostream>
#include <string>

//=================================================================
//Base Player Class (i.e. a GO the player controls)
//=================================================================

class Player : public CMOGO
{

public:
	Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Player();

	virtual void Tick(GameData* _GD) override;
	//string name = "";

	std::vector<std::shared_ptr<CMOGO>> projectiles;

protected:
	bool m_can_click = true, m_can_jump = true;
	float m_can_click_timer = 0, m_max_can_click_timer = 1.5, m_can_jump_timer = 0, m_max_can_jump_timer = 0.2f,
	speed = 250.0f, gravity = 500.0f, jumpspeed = 10000.0f;

};

#endif