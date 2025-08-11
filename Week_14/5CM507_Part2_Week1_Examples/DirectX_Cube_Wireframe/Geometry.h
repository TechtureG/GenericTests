#pragma once

constexpr auto ShaderFileName = L"shader.hlsl";
constexpr auto VertexShaderName = "VS";
constexpr auto PixelShaderName = "PS";

// Format of the constant buffer. This must match the format of the
// cbuffer structure in the shader

struct CBuffer
{
	Matrix		WorldViewProjection;
};

// Structure of a single vertex.  This must match the
// structure of the input vertex in the shader

struct Vertex
{
	Vector4		Position;
};

// The description of the vertex that is passed to CreateInputLayout.  This must
// match the format of the vertex above and the format of the input vertex in the shader

D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

// This example uses hard-coded vertices and indices for a cube. Usually, you will load the verticesa and indices from a model file. 
// We will see this later in the module. 


Vertex vertices[] =
{
	{ Vector4(-1.0f, -1.0f, -1.0f, 1.0f) },
	{ Vector4(-1.0f, +1.0f, -1.0f, 1.0f) },
	{ Vector4(+1.0f, +1.0f, -1.0f, 1.0f) },
	{ Vector4(+1.0f, -1.0f, -1.0f, 1.0f) },
	{ Vector4(-1.0f, -1.0f, +1.0f, 1.0f) },
	{ Vector4(-1.0f, +1.0f, +1.0f, 1.0f) },
	{ Vector4(+1.0f, +1.0f, +1.0f, 1.0f) },
	{ Vector4(+1.0f, -1.0f, +1.0f, 1.0f) }
};

UINT indices[] = {
	// front face
	0, 1, 2,
	0, 2, 3,

	// back face
	4, 6, 5,
	4, 7, 6,

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


