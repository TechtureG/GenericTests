#include "Camera.h"

Camera::Camera()
{
	// Initialise vectors used to create camera. We will do much
	// more in this class later
	_eyePosition = Vector3(0.0f, 50.0f, -50.0f);
	//_eyePosition = Vector3(0.0f, 150.0f, -300.0f);
	_focalPointPosition = Vector3(0.0f, 0.0f, 0.0f);
	_upVector = Vector3(0.0f, 1.0f, 0.0f);
	_viewMatrix = XMMatrixLookAtLH(_eyePosition, _focalPointPosition, _upVector);
}

const Matrix& Camera::GetViewMatrix() const
{
    return _viewMatrix;
}

