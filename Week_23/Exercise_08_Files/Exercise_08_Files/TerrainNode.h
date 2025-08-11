#pragma once
#include <vector>
#include "SceneNode.h"
#include "DirectXFramework.h"
#include "Framework.h"
#include "DirectXCore.h"
#include "WICTextureLoader.h"


class TerrainNode : public SceneNode
{
	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector2 TexCoords;
		Vector2 BlendMapTexCoords;
	};

	struct CBUFFER
	{
		Matrix		WorldViewProjection;
		Matrix		World;
		Vector4		CameraPosition;
		Vector4		LightVector;			// the light's vector
		Vector4		LightColor;				// the light's color
		Vector4		AmbientColor;			// the ambient light's color
		Vector4		DiffuseCoefficient;		// The diffuse reflection cooefficient
		Vector4		SpecularCoefficient;	// The specular reflection cooefficient
		float		Shininess;				// The shininess factor
		float		Opacity;				// The opacity (transparency) of the material. 0 = fully transparent, 1 = fully opaque
		Vector2		Padding;
	};

public:
	TerrainNode(wstring name);
	~TerrainNode();

	bool Initialise();
	void Render();
	void BuildGeometry();
	void BuildVertexLayout();
	void BuildShaders();
	void BuildConstantBuffer();
	void BuildTexture(const wchar_t* FileName);
	void BuildRendererStates();
	bool LoadHeightMap(const wchar_t* FileName);
	void LoadTerrainTextures();
	void GenerateBlendMap();
	void Shutdown(void);

private:
	// access the device
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

	ComPtr<ID3D11RasterizerState>	_rasteriserState;

	ComPtr<ID3D11ShaderResourceView> _texturesResourceView;
	ComPtr<ID3D11ShaderResourceView> _blendMapResourceView;

	vector <float>					_heightValues;
	bool							_useHeightMap;

	ComPtr<ID3D11ShaderResourceView> _texture;
	Vector4                         _diffuseColour;
	Vector4                         _specularColour;
	float							_shininess;
	float							_opacity;


	const unsigned int _xnumVert = 257;
	const unsigned int _znumVert = 257;
	const unsigned int _numVert = _xnumVert * _znumVert;
	const unsigned int _numInds = (_xnumVert - 1) * (_znumVert - 1) * 2 * 3;
	
};

