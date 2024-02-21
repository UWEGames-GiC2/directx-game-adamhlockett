#ifndef _FPS_CAMERA_
#define _FPS_CAMERA_
#include "camera.h"

//=================
//FPS style camera
//=================

class FPSCamera : public Camera
{
public:
	FPSCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, GameObject* _target, Vector3 _up, Vector3 _dpos);
	virtual ~FPSCamera();

	virtual void Tick(GameData* _GD) override;

protected:
	GameObject* m_targetObject; //I'm following this object
	Vector3	m_dpos; //I'll lurk this far behind and away from it
};

#endif
