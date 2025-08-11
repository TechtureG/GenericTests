#pragma once
#include "core.h"
#include "DirectXCore.h"

class Camera
{
public:
    Camera();

    const Matrix& GetViewMatrix() const;

private:
    Matrix  _viewMatrix;

    Vector3	_eyePosition;
    Vector3	_focalPointPosition;
    Vector3	_upVector;
};

