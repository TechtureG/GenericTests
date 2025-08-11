#pragma once
#include <vector>
#include "Framework.h"
#include "DirectXCore.h"

class Graphics2 : public Framework
{
public:
	Graphics2();

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

	D3D11_VIEWPORT					_screenViewport;

	bool GetDeviceAndSwapChain();
};

