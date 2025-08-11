#pragma once

#define ShaderFileName		L"shader.hlsl"
#define VertexShaderName	"VS"
#define PixelShaderName		"PS"

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
	Vector4		Colour;
};

// The description of the vertex that is passed to CreateInputLayout.  This must
// match the format of the vertex above and the format of the input vertex in the shader

D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// This example uses hard-coded vertices and indices for a cube. Usually, you will load the verticesa and indices from a model file. 
// We will see this later in the module. 


Vertex vertices[] =
{
	{ Vector4(-1.0f, -1.0f, -1.0f, 1.0f), Vector4(Colors::White)},
	{ Vector4(-1.0f, +1.0f, -1.0f, 1.0f), Vector4(Colors::Black)},
	{ Vector4(+1.0f, +1.0f, -1.0f, 1.0f), Vector4(Colors::Red)},
	{ Vector4(+1.0f, -1.0f, -1.0f, 1.0f), Vector4(Colors::Green)},
	{ Vector4(-1.0f, -1.0f, +1.0f, 1.0f), Vector4(Colors::Blue)},
	{ Vector4(-1.0f, +1.0f, +1.0f, 1.0f), Vector4(Colors::Yellow)},
	{ Vector4(+1.0f, +1.0f, +1.0f, 1.0f), Vector4(Colors::Cyan)},
	{ Vector4(+1.0f, -1.0f, +1.0f, 1.0f), Vector4(Colors::Magenta)}
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
	3, 3, 7
};

Vertex pyramidVertices[] =
{
	{ Vector4(-1.0f, -1.0f, -1.0f, 1.0f), Vector4(Colors::White)},
	{ Vector4(0.0f, +1.0f, 0.0f, 1.0f),   Vector4(Colors::Red)},
	{ Vector4(+1.0f, -1.0f, -1.0f, 1.0f), Vector4(Colors::Green)},
	{ Vector4(-1.0f, -1.0f, +1.0f, 1.0f), Vector4(Colors::Blue)},
	{ Vector4(+1.0f, -1.0f, +1.0f, 1.0f), Vector4(Colors::Magenta)}
};

UINT pyramidIndices[] = {
	// front face
	1, 2, 0,

	// back face
	1, 3, 4,

	// left face
	1, 0, 3,

	// right face
	1, 4, 2,

	// bottom face
	3, 0, 2,
	3, 2, 4
};

