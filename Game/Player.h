#ifndef _PLAYER_H_
#define _PLAYER_H_
#include "CMOGO.h" 

//=================================================================
//Base Player Class (i.e. a GO the player controls)
//=================================================================

class Player : public CMOGO
{

public:
	Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF);
	~Player();

	virtual void Tick(GameData* _GD) override;

protected:
	bool m_can_click = true;
	int m_can_click_timer = 0, m_max_can_click_timer = 120;
};

#endif