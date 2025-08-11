#include "SkyboxNode.h"

SkyboxNode::SkyboxNode(wstring name) : SceneNode(name)
{
	_name = name;
}

SkyboxNode::~SkyboxNode()
{

}

bool SkyboxNode::Initialise()
{
	// access the device
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	// access the divice context
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	BuildRendererStates();
	BuildDepthStencilState();
	BuildGeometry();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildTexture(L"SkyBox_Tex.jpg");

	return true;
}

void SkyboxNode::Render()
{
	Matrix _viewTransformation = DirectXFramework::GetDXFramework()->GetViewTransformation();
	Matrix _projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	Matrix completeTransformation = _worldTransformation * _viewTransformation * _projectionTransformation;

	CBUFFER cBuffer;
	//Set light
	//cBuffer.AmbientColor = Vector4(0.90f, 0.50f, 0.25f, 1.0f); //reddish sky
	//cBuffer.AmbientColor = Vector4(0.1f, 0.1f, 0.2f, 1.0f); //night sky
	cBuffer.AmbientColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f); //noon sky

	//Set transforms
	cBuffer.WorldViewProjection = completeTransformation;
	cBuffer.World = _worldTransformation;
	
	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf()); //constant buffer visible to Pixel Shader

	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	// Set the shaders to be used by the dc
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);

	// Set the texture to be used by the pixel shader
	_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());

	// Set the render state
	_deviceContext->RSSetState(_rasteriserState.Get());
	
	//Render the cube
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->DrawIndexed(36, 0, 0);

}


void SkyboxNode::Shutdown(void)
{
	CoUninitialize();
}

void SkyboxNode::BuildGeometry()
{
	// Create vertex buffer
	
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f),  XMFLOAT3(0.0f, 0.0f, +1.0f),  XMFLOAT2(189.0f / 720.0f, 189.0f / 540.0f) },    // side 1 front
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f),  XMFLOAT3(0.0f, 0.0f, +1.0f),  XMFLOAT2(189.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f),  XMFLOAT3(0.0f, 0.0f, +1.0f),  XMFLOAT2(354.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f),  XMFLOAT3(0.0f, 0.0f, +1.0f),  XMFLOAT2(354.0f / 720.0f, 189.0f / 540.0f) },

		{ XMFLOAT3(+1.0f, +1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f),  XMFLOAT2(544.0f / 720.0f, 189.0f / 540.0f) },    // side 2 rear
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f),  XMFLOAT2(544.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f),  XMFLOAT2(709.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f),  XMFLOAT3(0.0f, 0.0f, -1.0f),  XMFLOAT2(709.0f / 720.0f, 189.0f / 540.0f) },

		{ XMFLOAT3(-1.0f, +1.0f, +1.0f),  XMFLOAT3(0.0f, +1.0f, 0.0f),  XMFLOAT2(189.0f / 720.0f, 175.0f / 540.0f) },    // side 3 top
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f),  XMFLOAT3(0.0f, +1.0f, 0.0f),  XMFLOAT2(354.0f / 720.0f, 175.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f),  XMFLOAT3(0.0f, +1.0f, 0.0f),  XMFLOAT2(354.0f / 720.0f,  10.0f / 540.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f),  XMFLOAT3(0.0f, +1.0f, 0.0f),  XMFLOAT2(189.0f / 720.0f,  10.0f / 540.0f) },
		
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(189.0f / 720.0f, 367.0f / 540.0f) },    // side 4 bottom
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(189.0f / 720.0f, 532.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(354.0f / 720.0f, 367.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f),  XMFLOAT3(0.0f, -1.0f, 0.0f),  XMFLOAT2(354.0f / 720.0f, 532.0f / 540.0f) },
		
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f),  XMFLOAT3(+1.0f, 0.0f, 0.0f),  XMFLOAT2(367.0f / 720.0f, 189.0f / 540.0f) },    // side 5 right
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f),  XMFLOAT3(+1.0f, 0.0f, 0.0f),  XMFLOAT2(367.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, -1.0f),  XMFLOAT3(+1.0f, 0.0f, 0.0f),  XMFLOAT2(532.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f),  XMFLOAT3(+1.0f, 0.0f, 0.0f),  XMFLOAT2(532.0f / 720.0f, 189.0f / 540.0f) },

		{ XMFLOAT3(-1.0f, +1.0f, -1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2( 11.0f / 720.0f, 189.0f / 540.0f) },    // side 6 left
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2( 11.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2(176.0f / 720.0f, 354.0f / 540.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f),  XMFLOAT3(-1.0f, 0.0f, 0.0f),  XMFLOAT2(176.0f / 720.0f, 189.0f / 540.0f) },
	};

	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * 24;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &vertices;

	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));
	/*
	// Create the index buffer
	UINT indices[] = {
		0, 1, 2,       // side 1 front
		2, 3, 0,
		4, 5, 6,       // side 2 rear
		6, 7, 4,
		8, 9, 10,      // side 3 top
		10, 11, 8,
		12, 13, 14,    // side 4 bottom
		13, 15, 14,
		16, 17, 18,    // side 5 right
		18, 19, 16,
		20, 21, 22,    // side 6 left
		22, 23, 20,
	};
	*/
	// Create the index buffer in reverse order
	UINT indices[] = {
		2, 1, 0,       // side 1 front
		0, 3, 2,
		6, 5, 4,       // side 2 rear
		4, 7, 6,
		10, 9, 8,      // side 3 top
		8, 11, 10,
		14, 13, 12,    // side 4 bottom
		14, 15, 13,
		18, 17, 16,    // side 5 right
		16, 19, 18,
		22, 21, 20,    // side 6 left
		20, 23, 22,
	};

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * 36;
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &indices;

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void SkyboxNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
	#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"skyboxshader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VS", "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));
	//_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);

	// Compile pixel shader
	hr = D3DCompileFromFile(L"skyboxshader.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PS", "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
	//_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
}

void SkyboxNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
	_deviceContext->IASetInputLayout(_layout.Get());
}

void SkyboxNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void SkyboxNode::BuildTexture(const wchar_t* FileName)
{
	// Note that in order to use CreateWICTextureFromFile, we 
	// need to ensure we make a call to CoInitializeEx in our 
	// Initialise method (and make the corresponding call to 
	// CoUninitialize in the Shutdown method).  Otherwise, 
	// the following call will throw an exception
	ThrowIfFailed(CreateWICTextureFromFile(_device.Get(),
		_deviceContext.Get(),
		FileName, 
		nullptr,
		_texture.GetAddressOf()
	));
}

void SkyboxNode::BuildRendererStates()
{
	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	//rasteriserDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _rasteriserState.GetAddressOf()));
}

void SkyboxNode::BuildDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC stencilDesc;
	stencilDesc.DepthEnable = true;
	stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	stencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	stencilDesc.StencilEnable = false;
	stencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	stencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ThrowIfFailed(_device->CreateDepthStencilState(&stencilDesc, _stencilState.GetAddressOf()));
}