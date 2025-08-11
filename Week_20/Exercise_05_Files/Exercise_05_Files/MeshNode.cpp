#include "MeshNode.h"

bool MeshNode::Initialise()
{
	_resourceManager = DirectXFramework::GetDXFramework()->GetResourceManager();
	_mesh = _resourceManager->GetMesh(_modelName);
	
	if (_mesh == nullptr)
	{
		return false;
	}
	
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRendererState();
	return true;
}

void MeshNode::Shutdown()
{
	_resourceManager->ReleaseMesh(_modelName);
}

void MeshNode::BuildShaders()
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


void MeshNode::BuildVertexLayout()
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

void MeshNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

void MeshNode::BuildRendererState()
{
	// Set default and no cull rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
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


void MeshNode::Render()
{
	Matrix _viewTransformation = DirectXFramework::GetDXFramework()->GetViewTransformation();
	Matrix _projectionTransformation = DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	Matrix completeTransformation = _worldTransformation * _viewTransformation * _projectionTransformation;

	CBUFFER cBuffer;
	//Set light
	cBuffer.AmbientLightColour = Vector4(0.3f, 0.25f, 0.25f, 1.0f);
	cBuffer.DirectionalLightVector = Vector4(11.0f, 0.0f, -10.0f, 1.0f);
	cBuffer.DirectionalLightColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//Set default material colour to white;
	cBuffer.MaterialColour = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // for compatibility with used shader
	//Set transformation
	cBuffer.WorldViewProjection = completeTransformation;
	cBuffer.World = _worldTransformation;

	// Get submeshes
	size_t subMeshCount = _mesh->GetSubMeshCount();
	// Loop through all submeshes in the mesh, rendering them
	for (unsigned int i = 0; i < subMeshCount; i++)
	{
		shared_ptr<SubMesh> subMesh = _mesh->GetSubMesh(i);
		shared_ptr<Material> material = subMesh->GetMaterial(); // not used in current shader
		cBuffer.MaterialColour = material->GetDiffuseColour();  // for compatibility with used shader
		
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		_vertexBuffer = subMesh->GetVertexBuffer();
		_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
		_indexBuffer = subMesh->GetIndexBuffer();
		_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		// Update the constant buffer 
		_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
		_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);
		_texture = material->GetTexture();
		_deviceContext->PSSetShaderResources(0, 1, _texture.GetAddressOf());
		_deviceContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
		_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_deviceContext->DrawIndexed(static_cast<UINT>(subMesh->GetIndexCount()), 0, 0);
		
	}
	
	
}

