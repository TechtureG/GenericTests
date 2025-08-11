#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"
#include "SimpleMath.h"

using namespace SimpleMath; 

class DirectXApp : public Framework
{
public:
	DirectXApp();

	bool Initialise();
	void Update();
	void Render();
	void OnResize(WPARAM wParam);

private:
	ComPtr<ID3D11Device>			_device;
	ComPtr<ID3D11DeviceContext>		_deviceContext;
	ComPtr<IDXGISwapChain>			_swapChain;
	ComPtr<ID3D11Texture2D>			_depthStencilBuffer;
	ComPtr<ID3D11RenderTargetView>	_renderTargetView;
	ComPtr<ID3D11DepthStencilView>	_depthStencilView;

	D3D11_VIEWPORT					_screenViewport{ 0 };

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<ID3D11Buffer>			_constantBuffer;

	ComPtr<ID3D11RasterizerState>   _rasteriserState;

	Vector3							_eyePosition;
	Vector3							_focalPointPosition;
	Vector3							_upVector;

	// The default constructor for the Matrix class initialises the matrix to the Identity matrix.  

	Matrix							_worldTransformation;
	Matrix							_viewTransformation;
	Matrix							_projectionTransformation;

	int								_rotationAngle{ 0 };

	bool GetDeviceAndSwapChain();
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRasteriserState();
};

