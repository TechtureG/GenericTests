#pragma once
#include <vector>
#include "SceneNode.h"
#include <wrl/client.h>
#include <d3d11.h>
#include "WICTextureLoader.h"

using namespace DirectX;
using namespace Microsoft::WRL;

class CubeNode : public SceneNode
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
    CubeNode(std::wstring name);
    ~CubeNode();

    bool Initialise();
    void Render();
    void BuildGeometry();
    void BuildVertexLayout();
    void BuildTexture(const wchar_t* FileName);
    void BuildShaders();
    void BuildConstantBuffer();
    void BuildRasteriserState();

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

    ComPtr<ID3D11ShaderResourceView> _texture;
    
    Vector4                         _materialColour;

    unsigned int				    _indexCount{ 0 };

};
