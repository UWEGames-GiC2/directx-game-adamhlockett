#include "pch.h"
#include "FPSCamera.h"
#include "GameData.h"

FPSCamera::FPSCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, GameObject* _target, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_targetObject = _target;
	m_dpos = _dpos;
}

FPSCamera::FPSCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, std::shared_ptr<GameObject> _target, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_targetObject = _target.get();
	m_dpos = _dpos;
}

FPSCamera::~FPSCamera()
{

}

void FPSCamera::SetDPos(Vector3 _dpos) {
	m_dpos = _dpos;
}

void FPSCamera::SetTargetDisplacement(float _displacement) {
	m_target_displacement = -_displacement;
}

void FPSCamera::Tick(GameData* _GD)
{
	//Set up position of camera and target position of camera based on new position and orientation of target object
	Matrix rotCam = Matrix::CreateFromYawPitchRoll(m_targetObject->GetYaw(), m_targetObject->GetPitch(), 0.0f);
	m_target = { m_targetObject->GetPos().x, m_targetObject->GetPos().y + m_target_displacement, m_targetObject->GetPos().z};
	m_pos = m_target + Vector3::Transform(m_dpos, rotCam);

	//and then set up proj and view matrices
	Camera::Tick(_GD);
}

