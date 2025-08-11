#include "Mesh.h"

// Material methods

Material::Material(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity, ComPtr<ID3D11ShaderResourceView> texture)
{
	_materialName = materialName;
	_diffuseColour = diffuseColour;
	_specularColour = specularColour;
	_shininess = shininess;
	_opacity = opacity;
    _texture = texture;
}

Material::~Material(void)
{
}

// SubMesh methods

SubMesh::SubMesh(ComPtr<ID3D11Buffer> vertexBuffer,
				ComPtr<ID3D11Buffer> indexBuffer,
				size_t vertexCount,
				size_t indexCount,
				shared_ptr<Material> material,
				bool hasNormals,
				bool hasTexCoords)
{			
	_vertexBuffer = vertexBuffer;
	_indexBuffer = indexBuffer;
	_vertexCount = vertexCount;
	_indexCount = indexCount;
	_material = material;
	_hasNormals = hasNormals;
	_hasTexCoords = hasTexCoords;
}

SubMesh::~SubMesh(void)
{
}

// Mesh methods

size_t Mesh::GetSubMeshCount()
{
	return _subMeshList.size();
}

shared_ptr<SubMesh> Mesh::GetSubMesh(unsigned int i)
{
	if (i >= 0 && i < _subMeshList.size())
	{
		return _subMeshList[i];
	}
	else
	{
		return nullptr;
	}
}

void Mesh::AddSubMesh(shared_ptr<SubMesh> subMesh)
{
	_subMeshList.push_back(subMesh);
}

