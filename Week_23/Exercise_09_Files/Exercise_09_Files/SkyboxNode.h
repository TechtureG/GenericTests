#pragma once
#include <vector>
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "Framework.h"
#include "DirectXCore.h"
#include "WICTextureLoader.h"

class SkyboxNode : public SceneNode
{

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 TexCoords;
	};

	struct CBUFFER
	{
		Matrix		WorldViewProjection;
		Matrix		World;
		Vector4		AmbientColor;			// the sky's ambient color
	};

public:
	SkyboxNode(wstring name);
	~SkyboxNode();

	bool Initialise();
	void Render();
	void BuildGeometry();
	void BuildVertexLayout();
	void BuildShaders();
	void BuildConstantBuffer();
	void BuildRendererStates();
	void BuildDepthStencilState();
	void BuildTexture(const wchar_t* FileName);
	void Shutdown(void);

private:
	// access the device
	ComPtr<ID3D11Device>			_device;
	// access the device context
	ComPtr<ID3D11DeviceContext>		_deviceContext;

	//XMFLOAT4X4						_viewTransformation;
	//XMFLOAT4X4						_projectionTransformation;
	//XMFLOAT4X4						_worldTransformation;

	ComPtr<ID3DBlob>				_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>				_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>		_vertexShader;
	ComPtr<ID3D11PixelShader>		_pixelShader;
	ComPtr<ID3D11InputLayout>		_layout;
	ComPtr<IDXGISwapChain>			_swapChain;
	ComPtr<ID3D11RenderTargetView>	_renderTargetView;
	ComPtr<ID3D11DepthStencilView>	_depthStencilView;
	ComPtr<ID3D11Buffer>			_constantBuffer;
	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	ComPtr<ID3D11RasterizerState>	_rasteriserState;
	ComPtr<ID3D11DepthStencilState>	_stencilState;

	ComPtr<ID3D11ShaderResourceView> _texture;

	
};

