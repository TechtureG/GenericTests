#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "ResourceManager.h"

class MeshNode : public SceneNode
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
		Vector4		MaterialColour;
		Vector4		AmbientLightColour;
		Vector4		DirectionalLightColour;
		Vector4		DirectionalLightVector;
	};

public:
	MeshNode(wstring name, wstring modelName) : SceneNode(name) { _modelName = modelName; }

	bool Initialise();
	void Render();
	void Shutdown();

	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildRendererState();

private:
	/// access the device
	ComPtr<ID3D11Device>			_device;
	// access the device context
	ComPtr<ID3D11DeviceContext>		_deviceContext;

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
	ComPtr<ID3D11RasterizerState>   _rasteriserState;


	wstring							_modelName;
	shared_ptr<ResourceManager>		_resourceManager;
	shared_ptr<Mesh>				_mesh;

	ComPtr<ID3D11ShaderResourceView> _texture;

};

