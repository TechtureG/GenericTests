#include "Camera.h"

Vector4 defaultForward(0.0f, 0.0f, 1.0f, 0.0f);
Vector4 defaultRight(1.0f, 0.0f, 0.0f, 0.0f);
Vector4 defaultUp(0.0f, 1.0f, 0.0f, 0.0f);

Camera::Camera()
{
	_cameraPosition =Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	_moveLeftRight = 0.0f;
	_moveForwardBack = 0.0f;
	_cameraYaw = 0.0f;
	_cameraPitch = 0.0f;
	_cameraRoll = 0.0f;
}

void Camera::SetPitch(float pitch)
{
	_cameraPitch += XMConvertToRadians(pitch);
}

void Camera::SetTotalPitch(float pitch)
{
	_cameraPitch = XMConvertToRadians(pitch);
}

float Camera::GetPitch() const
{
	return XMConvertToDegrees(_cameraPitch);
}

void Camera::SetYaw(float yaw)
{
	_cameraYaw += XMConvertToRadians(yaw);
}

void Camera::SetTotalYaw(float yaw)
{
	_cameraYaw = XMConvertToRadians(yaw);
}

float Camera::GetYaw() const
{
	return XMConvertToDegrees(_cameraYaw);
}

void Camera::SetRoll(float roll)
{
	_cameraRoll += XMConvertToRadians(roll);
}

void Camera::SetTotalRoll(float roll)
{
	_cameraRoll = XMConvertToRadians(roll);
}

float Camera::GetRoll() const
{
	return XMConvertToDegrees(_cameraRoll);
}

void Camera::SetLeftRight(float leftRight)
{
	_moveLeftRight = leftRight;
}

void Camera::SetForwardBack(float forwardBack)
{
	_moveForwardBack = forwardBack;
}

const Matrix& Camera::GetViewMatrix() const
{
	return _viewMatrix;
}

const Vector4& Camera::GetCameraPosition() const
{
	return _cameraPosition;
}

void Camera::SetCameraPosition(float x, float y, float z)
{
	_cameraPosition = Vector4(x, y, z, 0.0f);
}

void Camera::Update(void)
{
	Vector4 cameraTarget;
	Vector4 cameraRight;
	Vector4 cameraForward;
	Vector4 cameraUp;

	// Yaw (rotation around the Y axis) will have an impact on the forward and right vectors
	Matrix cameraRotationYaw = XMMatrixRotationAxis(defaultUp, _cameraYaw);
	cameraRight = XMVector3TransformCoord(defaultRight, cameraRotationYaw);
	cameraForward = XMVector3TransformCoord(defaultForward, cameraRotationYaw);

	// Pitch (rotation around the X axis) impact the up and forward vectors
	Matrix cameraRotationPitch = XMMatrixRotationAxis(cameraRight, _cameraPitch);
	cameraUp = XMVector3TransformCoord(defaultUp, cameraRotationPitch);
	cameraForward = XMVector3TransformCoord(cameraForward, cameraRotationPitch);

	// Roll (rotation around the Z axis) will impact the Up and Right vectors
	Matrix cameraRotationRoll = XMMatrixRotationAxis(cameraForward, _cameraRoll);
	cameraUp = XMVector3TransformCoord(cameraUp, cameraRotationRoll);
	cameraRight = XMVector3TransformCoord(cameraRight, cameraRotationRoll);

	// Adjust the camera position by the appropriate amount forward/back and left/right
	_cameraPosition = _cameraPosition + _moveLeftRight * cameraRight + _moveForwardBack * cameraForward;

	// Reset the amount we are moving
	_moveLeftRight = 0.0f;
	_moveForwardBack = 0.0f;

	// Calculate a vector that tells us the direction the camera is looking in
	cameraTarget = _cameraPosition + cameraForward;

	// and calculate our view matrix
	_viewMatrix = XMMatrixLookAtLH(_cameraPosition, cameraTarget, cameraUp);
}
