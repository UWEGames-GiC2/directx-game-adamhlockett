#include "pch.h"
#include "Projectile.h"
#include <dinput.h>
#include "GameData.h"

Projectile::Projectile(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF) 
{
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.y = 10.0f;
	SetScale(Vector3::One * 0.005f);
	SetDrag(0.7);
	SetPhysicsOn(true);
}

Projectile::~Projectile(){

}

void Projectile::Tick(GameData* _GD)
{
	if (IsActive()) {
		lifetime += _GD->m_dt;
		if (lifetime > max_lifetime) 
		{
			SetActive(false);
			lifetime = 0;
		}
	}

	CMOGO::Tick(_GD);
}