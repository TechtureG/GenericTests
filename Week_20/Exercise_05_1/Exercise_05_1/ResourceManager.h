#pragma once
#include "Mesh.h"
#include <map>
#include <assimp\importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

using namespace Assimp;

struct MeshResourceStruct
{
	unsigned int			ReferenceCount;
	shared_ptr<Mesh>		MeshPointer;
};

typedef map<wstring, MeshResourceStruct>		MeshResourceMap;

struct MaterialResourceStruct
{
	unsigned int			ReferenceCount;
	shared_ptr<Material>	MaterialPointer;
};

typedef map<wstring, MaterialResourceStruct>	MaterialResourceMap;

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
				
	shared_ptr<Mesh>							GetMesh(wstring modelName);
	void										ReleaseMesh(wstring modelName);

	void										CreateMaterialFromTexture(wstring textureName);
    void										CreateMaterialWithNoTexture(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity);
    void										CreateMaterial(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity, wstring textureName);
	shared_ptr<Material>						GetMaterial(wstring materialName);
	void										ReleaseMaterial(wstring materialName);

private:
	MeshResourceMap								_meshResources;
	MaterialResourceMap							_materialResources;

	ComPtr<ID3D11Device>						_device;
	ComPtr<ID3D11DeviceContext>					_deviceContext;

	shared_ptr<Mesh>							LoadModelFromFile(wstring modelName);
    void										InitialiseMaterial(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity, wstring textureName);
};

