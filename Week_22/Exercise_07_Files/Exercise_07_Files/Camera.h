#pragma once
#include "core.h"
#include "DirectXCore.h"

class Camera
{
public:
    Camera();

    void Update();
    void UpdateViewMatrix();
    const Matrix& GetViewMatrix() const;
    const Vector4& GetCameraPosition() const;
    void SetCameraPosition(float x, float y, float z);
    void SetPitch(float pitch);
    void SetTotalPitch(float pitch);
    float GetPitch() const;
    void SetYaw(float yaw);
    void SetTotalYaw(float yaw);
    float GetYaw() const;
    void SetRoll(float roll);
    void SetTotalRoll(float roll);
    float GetRoll() const;
    void SetLeftRight(float leftRight);
    void SetForwardBack(float forwardBack);

private:
    Matrix  _viewMatrix;

    Vector4	_cameraPosition;
    Vector4	_cameraTarget;
    Vector4	_cameraUp;

    float   _moveLeftRight;
    float   _moveForwardBack;

    float   _cameraYaw;
    float   _cameraPitch;
    float   _cameraRoll;
};

