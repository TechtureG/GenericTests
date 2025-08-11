#include "CubeNode.h"
#include "DirectXFramework.h"

CubeNode::CubeNode(std::wstring name) : SceneNode(name) 
{
	_name = name;
	_materialColour = Vector4(0.0f, 1.0f, 1.0f, 1.0f);
}

CubeNode::~CubeNode() {}

bool CubeNode::Initialise()
{
	// access the device
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	// access the device context
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	BuildGeometry();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRasteriserState();

	return true;
}

void CubeNode::Render()
{
	//DirectXFramework::GetDXFramework()->GetCamera()->SetViewMatrix();
	Matrix _viewTransformation = DirectXFramework::GetDXFramework()->GetViewTransformation();
	Matrix _projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	Matrix completeTransformation = _worldTransformation * _viewTransformation * _projectionTransformation;
	
	CBUFFER cBuffer;
	//Set light
	cBuffer.AmbientLightColour = Vector4(0.3f, 0.25f, 0.25f, 1.0f);
	cBuffer.DirectionalLightVector = Vector4(11.0f, 0.0f, -10.0f, 1.0f);
	cBuffer.DirectionalLightColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Set material
	cBuffer.MaterialColour = _materialColour;
	//Set transforms
	cBuffer.WorldViewProjection = completeTransformation;
	cBuffer.World = _worldTransformation;

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	//Render the cube
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->DrawIndexed(_indexCount, 0, 0);
}

void CubeNode::BuildGeometry()
{
	/*
	// Cube 1st version
	// Create vertex buffer
	Vertex vertices[] =
	{
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },    // 0
		{ Vector3(-1.0f, +1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },	// 1
		{ Vector3(+1.0f, +1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },	// 2
		{ Vector3(+1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, 0.0f) },	// 3

		{ Vector3(-1.0f, -1.0f, +1.0f), Vector3(0.0f, 0.0f, 0.0f) },    // 4
		{ Vector3(-1.0f, +1.0f, +1.0f), Vector3(0.0f, 0.0f, 0.0f) },	// 5
		{ Vector3(+1.0f, +1.0f, +1.0f), Vector3(0.0f, 0.0f, 0.0f) },	// 6
		{ Vector3(+1.0f, -1.0f, +1.0f), Vector3(0.0f, 0.0f, 0.0f) }		// 7
	};

	// Create the index buffer
	UINT indices[] = {
		// front face
	0, 1, 2,
	0, 2, 3,

	// back face
	4, 7, 5,
	5, 7, 6,

	// left face
	4, 5, 1,
	4, 1, 0,

	// right face
	3, 2, 6,
	3, 6, 7,

	// top face
	1, 5, 6,
	1, 6, 2,

	// bottom face
	4, 0, 3,
	4, 3, 7
	};
	*/

	// Cube 2nd version
	// Create vertex buffer
	Vertex vertices[] =
	{
		{ Vector3(-1.0f, -1.0f, 1.0f),  Vector3(0.0f, 0.0f, 1.0f) },    // side 1
		{ Vector3(1.0f, -1.0f, 1.0f),   Vector3(0.0f, 0.0f, 1.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),   Vector3(0.0f, 0.0f, 1.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),    Vector3(0.0f, 0.0f, 1.0f) },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f) },    // side 2
		{ Vector3(-1.0f, 1.0f, -1.0f),  Vector3(0.0f, 0.0f, -1.0f) },
		{ Vector3(1.0f, -1.0f, -1.0f),  Vector3(0.0f, 0.0f, -1.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),   Vector3(0.0f, 0.0f, -1.0f) },

		{ Vector3(-1.0f, 1.0f, -1.0f),  Vector3(0.0f, 1.0f, 0.0f) },    // side 3
		{ Vector3(-1.0f, 1.0f, 1.0f),   Vector3(0.0f, 1.0f, 0.0f) },
		{ Vector3(1.0f, 1.0f, -1.0f),   Vector3(0.0f, 1.0f, 0.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),    Vector3(0.0f, 1.0f, 0.0f) },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f) },    // side 4
		{ Vector3(1.0f, -1.0f, -1.0f),  Vector3(0.0f, -1.0f, 0.0f) },
		{ Vector3(-1.0f, -1.0f, 1.0f),  Vector3(0.0f, -1.0f, 0.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),   Vector3(0.0f, -1.0f, 0.0f) },

		{ Vector3(1.0f, -1.0f, -1.0f),  Vector3(1.0f, 0.0f, 0.0f) },    // side 5
		{ Vector3(1.0f, 1.0f, -1.0f),   Vector3(1.0f, 0.0f, 0.0f) },
		{ Vector3(1.0f, -1.0f, 1.0f),   Vector3(1.0f, 0.0f, 0.0f) },
		{ Vector3(1.0f, 1.0f, 1.0f),    Vector3(1.0f, 0.0f, 0.0f) },

		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(-1.0f, 0.0f, 0.0f) },    // side 6
		{ Vector3(-1.0f, -1.0f, 1.0f),  Vector3(-1.0f, 0.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, -1.0f),  Vector3(-1.0f, 0.0f, 0.0f) },
		{ Vector3(-1.0f, 1.0f, 1.0f),   Vector3(-1.0f, 0.0f, 0.0f) },
	};

	// Create the index buffer
	UINT indices[] = {
		0, 1, 2,       // side 1
		2, 1, 3,
		4, 5, 6,       // side 2
		6, 5, 7,
		8, 9, 10,      // side 3
		10, 9, 11,
		12, 13, 14,    // side 4
		14, 13, 15,
		16, 17, 18,    // side 5
		18, 17, 19,
		20, 21, 22,    // side 6
		22, 21, 23,
	};

	_indexCount = ARRAYSIZE(indices);


	// Calculate vertex normals
	vector<int> vertexContributingCount(ARRAYSIZE(vertices), 0);
	int polygonCount = static_cast<int>(ARRAYSIZE(indices)) / 3;
	for (int i = 0; i < polygonCount; i++)
	{
		unsigned int index0 = indices[i * 3];
		unsigned int index1 = indices[i * 3 + 1];
		unsigned int index2 = indices[i * 3 + 2];
		Vector3 u = vertices[index1].Position - vertices[index0].Position;
		Vector3 v = vertices[index2].Position - vertices[index0].Position;
		//Vector3 normal = u.Cross(v); // CCW
		Vector3 normal = v.Cross(u); // CW
		vertices[index0].Normal += normal;
		vertexContributingCount[index0]++;
		vertices[index1].Normal += normal;
		vertexContributingCount[index1]++;
		vertices[index2].Normal += normal;
		vertexContributingCount[index2]++;
	}
	// Now divide the vertex normals by the contributing counts and normalise
	for (int i = 0; i < ARRAYSIZE(vertices); i++)
	{
		vertices[i].Normal /= (float)vertexContributingCount[i];
		vertices[i].Normal.Normalize();
	}

	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
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

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * ARRAYSIZE(indices);
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

void CubeNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.
	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
	_deviceContext->IASetInputLayout(_layout.Get());
}

void CubeNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"shader.hlsl",
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
	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);

	// Compile pixel shader
	hr = D3DCompileFromFile(L"shader.hlsl",
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
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
}

void CubeNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void CubeNode::BuildRasteriserState()
{	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	// The following tells the rasteriser to apply backfece culling.  For not doing so, set it to D3D11_CULL_NONE
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	//rasteriserDesc.CullMode = D3D11_CULL_NONE;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	// The following tells the rasteriser to draw a wireframe model.  For solid models, set it to D3D11_FILL_SOLID
	rasteriserDesc.FillMode = D3D11_FILL_WIREFRAME;
	//rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _rasteriserState.GetAddressOf()));
}






