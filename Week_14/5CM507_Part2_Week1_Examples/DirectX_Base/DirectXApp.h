#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"

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

	bool GetDeviceAndSwapChain();
};

