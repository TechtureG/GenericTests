#include "TerrainNode.h"
#include <fstream>

TerrainNode::TerrainNode(wstring name) : SceneNode(name)
{
	_name = name;
	_materialColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	_useHeightMap = false;
}

TerrainNode::~TerrainNode()
{

}

bool TerrainNode::Initialise()
{
	// access the device
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	// access the divice context
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	_useHeightMap = LoadHeightMap(L"Example_HeightMap.raw");
	BuildRendererStates();
	BuildGeometry();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildTexture(L"white.png");
	//BuildTexture(L"dirt.jpg");
	//BuildTexture(L"grass.jpg");
	//BuildTexture(L"Wood.png"); 
	//BuildTexture(L"Concrete.png");
	//BuildTexture(L"RGB_Colormap.png");

	return true;
}



void TerrainNode::Shutdown(void)
{
	CoUninitialize();
}

void TerrainNode::Render()
{
	Matrix _viewTransformation = DirectXFramework::GetDXFramework()->GetViewTransformation();
	Matrix _projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();

	Matrix completeTransformation = _worldTransformation * _viewTransformation * _projectionTransformation;

	CBUFFER cBuffer;
	//Set light
	cBuffer.AmbientLightColour = Vector4(0.3f, 0.25f, 0.25f, 1.0f);
	cBuffer.DirectionalLightVector = Vector4(0.0f, 2.0f, -10.0f, 1.0f);
	cBuffer.DirectionalLightColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Set material
	cBuffer.MaterialColour = _materialColour;
	//Set transforms
	cBuffer.WorldViewProjection = completeTransformation;
	cBuffer.World = _worldTransformation;

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	// Set the texture to be used by the pixel shader
	_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());

	// Set the render state
	_deviceContext->RSSetState(_rasteriserState.Get());

	//Render the terrain
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->DrawIndexed(_numInds, 0, 0);

}

void TerrainNode::BuildGeometry()
{
	struct TerVertex
	{
		Vector3 Pos;
		Vector3 Norm;
		Vector2 Tex;
	};

	TerVertex* terrainVerts = new TerVertex[_numVert];
	unsigned int * terrainInds = new unsigned int[_numInds];


	float x0 = 0.0f;
	float y0 = -80.0f;
	float z0 = 0.0f;
	float dx = 1.0f;
	float dy = 0.1f;
	float dz = 1.0f;
	float height = 80.0f;

	x0 = -((_xnumVert / 2) * dx);
	z0 = -((_znumVert / 2) * dz);
	
	//_useHeightMap = false;
	srand(0);

	for (unsigned int  j = 0; j < _znumVert; j++)
		for (unsigned int  i = 0; i < _xnumVert; i++)
		{
			unsigned int inx = j * _xnumVert + i; // index to height values list
			terrainVerts[inx].Pos.x = x0 + (float) i * dx;		
			if (_useHeightMap)
				terrainVerts[inx].Pos.y = y0 + _heightValues[inx] * height;
			else
				terrainVerts[inx].Pos.y = y0; // flat surface
			//terrainVerts[inx].Pos.y = terrainVerts[inx].Pos.y + (rand() % 10) * dy; // add random roughness
			terrainVerts[inx].Pos.z = z0 + j * dz;

			//Normals will be calculated
			terrainVerts[inx].Norm.x = 0.0f;
			terrainVerts[inx].Norm.y = 1.0f;
			terrainVerts[inx].Norm.z = 0.0f;

			//Stretched Texture
			//terrainVerts[inx].Tex.x = i / (float)_xnumVert;
			//terrainVerts[inx].Tex.y = j / (float)_znumVert;

			//Tiling Texture
			//terrainVerts[inx].Tex.x = (float)(i % 2);
			//terrainVerts[inx].Tex.y = (float)(j % 2);
			terrainVerts[inx].Tex.x = (float)(i % 24) / 24.0f;
			terrainVerts[inx].Tex.y = (float)(j % 24) / 24.0f;

			//Random Sampling of Texture
			//terrainVerts[inx].Tex.x = (rand() % 1000) / 1000.0f;
			//terrainVerts[inx].Tex.y = (rand() % 1000) / 1000.0f;
		}

	for (unsigned int  j = 0; j < (_znumVert - 1); j++)
		for (unsigned int  i = 0; i < (_xnumVert - 1); i++)
		{
			unsigned int  inx = (j * (_xnumVert - 1) + i) * 2 * 3;
			terrainInds[inx] = j * _xnumVert + i;
			terrainInds[inx + 1] = (j + 1) * _xnumVert + i;
			terrainInds[inx + 2] = j * _xnumVert + i + 1;
			terrainInds[inx + 3] = j * _xnumVert + i + 1;
			terrainInds[inx + 4] = (j + 1) * _xnumVert + i;
			terrainInds[inx + 5] = (j + 1) * _xnumVert + i + 1;	
		}
	
	// Calculate vertex normals
	unsigned int* vertexContributingCount = new unsigned int[_numVert];
	Vector3 u, v, normal;

	for (unsigned int  index = 0; index < _numVert; index++)
	{
		vertexContributingCount[index] = 0;
	}
	for (unsigned int  j = 0; j < (_znumVert - 1); j++)
		for (unsigned int  i = 0; i < (_xnumVert - 1); i++)
		{
			unsigned int index = (j * (_xnumVert - 1) + i) * 3 * 2;
			
			unsigned int index0 = terrainInds[index];
			unsigned int index1 = terrainInds[index + 1];
			unsigned int index2 = terrainInds[index + 2];
			u = XMVectorSet(terrainVerts[index1].Pos.x - terrainVerts[index0].Pos.x,
				terrainVerts[index1].Pos.y - terrainVerts[index0].Pos.y,
				terrainVerts[index1].Pos.z - terrainVerts[index0].Pos.z,
				1.0f);
			v = XMVectorSet(terrainVerts[index2].Pos.x - terrainVerts[index0].Pos.x,
				terrainVerts[index2].Pos.y - terrainVerts[index0].Pos.y,
				terrainVerts[index2].Pos.z - terrainVerts[index0].Pos.z,
				1.0f);
			normal = XMVector3Cross(u, v); //CCW
			//normal = XMVector3Cross(v, u); //CW
			XMStoreFloat3(&terrainVerts[index0].Norm, XMVectorAdd(XMLoadFloat3(&terrainVerts[index0].Norm), normal));
			vertexContributingCount[index0]++;
			XMStoreFloat3(&terrainVerts[index1].Norm, XMVectorAdd(XMLoadFloat3(&terrainVerts[index1].Norm), normal));
			vertexContributingCount[index1]++;
			XMStoreFloat3(&terrainVerts[index2].Norm, XMVectorAdd(XMLoadFloat3(&terrainVerts[index2].Norm), normal));
			vertexContributingCount[index2]++;
			
			unsigned int index3 = terrainInds[index + 3];
			unsigned int index4 = terrainInds[index + 4];
			unsigned int index5 = terrainInds[index + 5];
			u = XMVectorSet(terrainVerts[index4].Pos.x - terrainVerts[index3].Pos.x,
				terrainVerts[index4].Pos.y - terrainVerts[index3].Pos.y,
				terrainVerts[index4].Pos.z - terrainVerts[index3].Pos.z,
				1.0f);
			v = XMVectorSet(terrainVerts[index5].Pos.x - terrainVerts[index3].Pos.x,
				terrainVerts[index5].Pos.y - terrainVerts[index3].Pos.y,
				terrainVerts[index5].Pos.z - terrainVerts[index3].Pos.z,
				1.0f);
			normal = XMVector3Cross(u, v); //CCW
			//normal = XMVector3Cross(v, u); //CW
			XMStoreFloat3(&terrainVerts[index3].Norm, XMVectorAdd(XMLoadFloat3(&terrainVerts[index3].Norm), normal));
			vertexContributingCount[index3]++;
			XMStoreFloat3(&terrainVerts[index4].Norm, XMVectorAdd(XMLoadFloat3(&terrainVerts[index4].Norm), normal));
			vertexContributingCount[index4]++;
			XMStoreFloat3(&terrainVerts[index5].Norm, XMVectorAdd(XMLoadFloat3(&terrainVerts[index5].Norm), normal));
			vertexContributingCount[index5]++;
		}
	// Now divide the vertex normals by the contributing counts and normalise
	for (unsigned int  index = 0; index < _numVert; index++)
	{
		Vector3 vertexNormal = XMLoadFloat3(&terrainVerts[index].Norm);
		if (vertexContributingCount[index] == 0)
			XMStoreFloat3(&terrainVerts[index].Norm, XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
		else XMStoreFloat3(&terrainVerts[index].Norm, vertexNormal / (float)vertexContributingCount[index]);
	}
	
	// Setup the structure that specifies how big the vertex
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * _numVert;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the vertices from
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = terrainVerts;// &terrainVerts;

	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(int) * _numInds;
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = terrainInds;// &terrainInds;

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));

	delete[] terrainVerts;
	delete[] terrainInds;

}

void TerrainNode::BuildShaders()
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

void TerrainNode::BuildVertexLayout()
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

void TerrainNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void TerrainNode::BuildTexture(const wchar_t* FileName)
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

bool TerrainNode::LoadHeightMap(const wchar_t* FileName)
{
	//size of Example_HeightMap.raw
	unsigned int numberOfXPoints = 1024;
	unsigned int numberOfZPoints = 1024;
	unsigned int mapSize = numberOfXPoints * numberOfZPoints;  // 1024 * 1024;//

	USHORT* rawFileValues = new USHORT[mapSize];

	ifstream inputHeightMap;
	//inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
	inputHeightMap.open(FileName, std::ios_base::binary);
	if (!inputHeightMap)
	{
		return false;
	}
	inputHeightMap.read((char*)rawFileValues, mapSize);
	inputHeightMap.close();

	// Normalise BYTE values to the range 0.0f - 1.0f;
	// Saving only what we need from a top-left rectangular area
	for (UINT j = 0; j < _znumVert; j++)
		for (UINT i = 0; i < _xnumVert; i++)
		{
			int inx = j * numberOfXPoints + i;
			_heightValues.push_back((float)rawFileValues[inx] / 65536.0f); //2 bytes = 256*256 = 65536
		}
	delete[] rawFileValues;
	return true;
}

void TerrainNode::BuildRendererStates()
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