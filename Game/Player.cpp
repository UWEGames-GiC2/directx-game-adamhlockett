#include "pch.h"
#include "Player.h"
#include <dinput.h>
#include "GameData.h"

Player::Player(string _fileName, ID3D11Device* _pd3dDevice, IEffectFactory* _EF) : CMOGO(_fileName, _pd3dDevice, _EF)
{
	//any special set up for Player goes here
	m_fudge = Matrix::CreateRotationY(XM_PI);

	m_pos.y = 10.0f;

	SetDrag(2);
	SetPhysicsOn(true);
}

Player::~Player()
{
	//tidy up anything I've created
}


void Player::Tick(GameData* _GameData)
{
	//switch (_GameData->m_GS)
	//{
		//case GS_PLAY_MAIN_CAM:
		//{
		//	//MOUSE CONTROL SCHEME HERE
		//	//float speed = 10.0f;
		//	//m_acc.x += speed * _GameData->m_MS.x;
		//	//m_acc.z += speed * _GameData->m_MS.y;
		//	//break;
		//}
		//case GS_PLAY_FPS_CAM:
		//{
			//TURN AND FORWARD CONTROL HERE
			float speed = 250.0f;
			float gravity = 400.0f;
			float jumpspeed = 2000.0f;
			Vector3 forwardMove = speed * Vector3::Forward;
			Vector3 sidewardMove = speed * Vector3::Left;
			Matrix rotMove = Matrix::CreateRotationY(m_yaw);
			auto kb = Keyboard::Get().GetState();
			auto ms = Mouse::Get().GetState();
			forwardMove = Vector3::Transform(forwardMove, rotMove);
			sidewardMove = Vector3::Transform(sidewardMove, rotMove);
			int rotationBounds = 85;
			if (m_pitch > XMConvertToRadians(rotationBounds)) m_pitch = XMConvertToRadians(rotationBounds);
			if (m_pitch < XMConvertToRadians(-rotationBounds)) m_pitch = XMConvertToRadians(-rotationBounds);
			if (ms.leftButton)
			{
				if (m_can_click) {
					//_GameData->FireProj();
					_GameData->m_hand_anim = true;
				}
				m_can_click = false;
			}
			if (kb.W)
			{
				m_acc += forwardMove;
			}
			if (kb.S)
			{
				m_acc -= forwardMove;
			}
			if (kb.A)
			{
				m_acc += sidewardMove;
			}
			if (kb.D)
			{
				m_acc -= sidewardMove;
			}
			if (kb.Space)
			{
				m_acc.y += jumpspeed;
			}
			m_acc.y -= gravity;

			if (!m_can_click) {
				m_can_click_timer++;
				if (m_can_click_timer >= m_max_can_click_timer) {
					m_can_click = true;
					m_can_click_timer = 0;
				}
			}
			

			float sensitivity = 0.5f;
			float rotSpeed = sensitivity * _GameData->m_dt;
			m_yaw -= rotSpeed * _GameData->m_MS.x;
			m_pitch -= rotSpeed * _GameData->m_MS.y;
			//break;
		//}
	//}

	//change orinetation of player
	
	//if (_GD->m_KBS.A)
	//{
	//	m_yaw += rotSpeed;
	//}
	//if (_GD->m_KBS.D)
	//{
	//	m_yaw -= rotSpeed;
	//}

	//move player up and down
	//if (_GameData->m_KBS.R)
	//{
	//	m_acc.y += 40.0f;
	//}
	//
	//if (_GameData->m_KBS.F)
	//{
	//	m_acc.y -= 40.0f;
	//}

	//limit motion of the player
	float length = m_pos.Length();
	float maxLength = 500.0f;
	if (length > maxLength)
	{
		m_pos.Normalize();
		m_pos *= maxLength;
		m_vel *= -0.9; //VERY simple bounce back
	}

	//apply my base behaviour
	CMOGO::Tick(_GameData);
}