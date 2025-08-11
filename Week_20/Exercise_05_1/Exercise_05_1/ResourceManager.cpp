#include "ResourceManager.h"
#include "DirectXFramework.h"
#include <sstream>
#include "WICTextureLoader.h"
#include <locale>
#include <codecvt>

#pragma comment(lib, "Assimp/lib/release/assimp-vc143-mt.lib")

using namespace Assimp;

//-------------------------------------------------------------------------------------------
// Utility functions to convert from wstring to string and back
// Copied from https://stackoverflow.com/questions/4804298/how-to-convert-wstring-into-string

wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

//-------------------------------------------------------------------------------------------

ResourceManager::ResourceManager()
{
	_device = DirectXFramework::GetDXFramework()->GetDevice();
	_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();
}

ResourceManager::~ResourceManager(void)
{
}

shared_ptr<Mesh> ResourceManager::GetMesh(wstring modelName)
{
	// CHeck to see if the mesh has already been loaded
	MeshResourceMap::iterator it = _meshResources.find(modelName);
	if (it != _meshResources.end())
	{
		// Update reference count and return pointer to existing mesh
		it->second.ReferenceCount++;
		return it->second.MeshPointer;
	}
	else
	{
		// This is the first request for this model.  Load the mesh and
		// save a reference to it.
		shared_ptr<Mesh> mesh = LoadModelFromFile(modelName);
		if (mesh != nullptr)
		{
			MeshResourceStruct resourceStruct;
			resourceStruct.ReferenceCount = 1;
			resourceStruct.MeshPointer = mesh;
			_meshResources[modelName] = resourceStruct;
			return mesh;
		}
		else
		{
			return nullptr;
		}
	}
}

void ResourceManager::ReleaseMesh(wstring modelName)
{
	MeshResourceMap::iterator it = _meshResources.find(modelName);
	if (it != _meshResources.end())
	{
		it->second.ReferenceCount--;
		if (it->second.ReferenceCount == 0)
		{
			// Release any materials used by this mesh
			shared_ptr<Mesh> mesh = it->second.MeshPointer;
			unsigned int subMeshCount = static_cast<unsigned int>(mesh->GetSubMeshCount());
			// Loop through all submeshes in the mesh
			for (unsigned int i = 0; i < subMeshCount; i++)
			{
				shared_ptr<SubMesh> subMesh = mesh->GetSubMesh(i);
				wstring materialName = subMesh->GetMaterial()->GetMaterialName();
				ReleaseMaterial(materialName);
			}
			// If no other nodes are using this mesh, remove it frmo the map
			// (which will also release the resources).
			it->second.MeshPointer = nullptr;
			_meshResources.erase(modelName);
		}
	}
}

void ResourceManager::CreateMaterialFromTexture(wstring textureName)
{
    // We have no diffuse or specular colours here since we are just building a default material structure
    // based on a provided texture. Just use the texture name as the material name in this case
    InitialiseMaterial(textureName, 
                       Vector4(1.0f, 1.0f, 1.0f, 1.0f), 
                       Vector4(0.0f, 0.0f, 0.0f, 1.0f),
                       0,
	   			       1.0f,
                       textureName);
}

void ResourceManager::CreateMaterialWithNoTexture(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity)
{
    InitialiseMaterial(materialName, diffuseColour, specularColour, shininess, opacity, L"");
}

void ResourceManager::CreateMaterial(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity, wstring textureName)
{
    InitialiseMaterial(materialName, diffuseColour, specularColour, shininess, opacity, textureName);
}

shared_ptr<Material> ResourceManager::GetMaterial(wstring materialName)
{
	// This works a bit different to the GetMesh method.  We can only find
	// materials we have previously created (usually when the mesh was loaded
	// from the file).
	MaterialResourceMap::iterator it = _materialResources.find(materialName);
	if (it != _materialResources.end())
	{
		it->second.ReferenceCount++;
		return it->second.MaterialPointer;
	}
	else
	{
		// Material not previously created.
		return nullptr;
	}
}

void ResourceManager::ReleaseMaterial(wstring materialName)
{
	MaterialResourceMap::iterator it = _materialResources.find(materialName);
	if (it != _materialResources.end())
	{
		it->second.ReferenceCount--;
		if (it->second.ReferenceCount == 0)
		{
			it->second.MaterialPointer = nullptr;
			_meshResources.erase(materialName);
		}
	}
}

void ResourceManager::InitialiseMaterial(wstring materialName, Vector4 diffuseColour, Vector4 specularColour, float shininess, float opacity, wstring textureName)
{
	MaterialResourceMap::iterator it = _materialResources.find(materialName);
	if (it == _materialResources.end())
	{
		// We are creating the material for the first time
		ComPtr<ID3D11ShaderResourceView> texture;
		if (textureName.size() > 0)
		{
			// A texture was specified.  Try to load it.
			if (FAILED(CreateWICTextureFromFile(_device.Get(),
											    _deviceContext.Get(),
												textureName.c_str(),
												nullptr,
												texture.GetAddressOf()
												)))
			{
				texture = nullptr;
			}
		}
		else
		{
			texture = nullptr;;
		}
		if (texture == nullptr)
		{
			// Create a default white texture for use when texture file is not available. 
			// This happens for materials that do not provide a texture, and creates problems 
			// unless we provide a totally different shader just for those cases.  
			// That might be more efficient, but is a lot of work at this stage for little gain.
			// If neither white.png is available, then the default texture will be null, i.e. black.  
			if (FAILED(CreateWICTextureFromFile(_device.Get(),
												_deviceContext.Get(),
												L"white.png",
												nullptr,
												texture.GetAddressOf()
												)))
			{
				texture = nullptr;
			}
		}
		shared_ptr<Material> material = make_shared<Material>(materialName, diffuseColour, specularColour, shininess, opacity, texture);
		MaterialResourceStruct resourceStruct;
		resourceStruct.ReferenceCount = 0;
		resourceStruct.MaterialPointer = material;
		_materialResources[materialName] = resourceStruct;
	}
}

shared_ptr<Mesh> ResourceManager::LoadModelFromFile(wstring modelName)
{
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	wstring* materials = nullptr;

	Importer importer;

	unsigned int postProcessSteps = aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded;
	string modelNameUTF8 = ws2s(modelName);
	const aiScene* scene = importer.ReadFile(modelNameUTF8.c_str(), postProcessSteps);
	if (!scene)
	{
		// If failed to load, there is nothing to do
		return nullptr;
	}
	if (!scene->HasMeshes())
	{
		//If there are no meshes, then there is nothing to do.
		return nullptr;
	}
	if (scene->HasMaterials())
	{
		// We need to find the directory part of the model name since we will need to add it to any texture names. 
		// There is definately a more elegant and accurate way to do this using Windows API calls, but this is a quick
		// and dirty approach
		string::size_type slashIndex = modelNameUTF8.find_last_of("\\");
		string directory;
		if (slashIndex == string::npos)
		{
			directory = ".";
		}
		else if (slashIndex == 0)
		{
			directory = "/";
		}
		else
		{
			directory = modelNameUTF8.substr(0, slashIndex);
		}
		// Let's deal with the materials/textures first
		materials = new wstring[scene->mNumMaterials];
		for (unsigned int i = 0; i < scene->mNumMaterials; i++)
		{
			// Get the core material properties.  Ideally, we would be looking for more information
			// e.g. emissive colour, etc.  This is a task for later.
			aiMaterial* material = scene->mMaterials[i];

			aiColor3D diffuseColour(0.0f, 0.0f, 0.0f);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColour);
			aiColor3D specularColour(0.0f, 0.0f, 0.0f);
			material->Get(AI_MATKEY_COLOR_SPECULAR, specularColour);
			float defaultShininess = 0.0f;
			float& shininess = defaultShininess;
			material->Get(AI_MATKEY_SHININESS, shininess);
			float defaultOpacity = 1.0f;
			float& opacity = defaultOpacity;
			material->Get(AI_MATKEY_OPACITY, opacity);
			string fullTextureNamePath = "";
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				aiString textureName;
				float blendFactor;
				aiTextureOp blendOp;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &textureName, NULL, NULL, &blendFactor, &blendOp, NULL) == AI_SUCCESS)
				{
					// Get full path to texture by prepending the same folder as included in the model name. This
					// does assume that textures are in the same folder as the model files
					fullTextureNamePath = directory + "\\" + textureName.data;
				}
			}
			// Now create a unique name for the material based on the model name and loop count
			stringstream materialNameStream;
			materialNameStream << modelNameUTF8 << i;
			string materialName = materialNameStream.str();
			wstring materialNameWS = s2ws(materialName);
			CreateMaterial(materialNameWS,
				Vector4(diffuseColour.r, diffuseColour.g, diffuseColour.b, 1.0f),
				Vector4(specularColour.r, specularColour.g, specularColour.b, 1.0f),
				shininess,
				opacity,
				s2ws(fullTextureNamePath));
			materials[i] = materialNameWS;
		}
	}
	// Now we have created all of the materials, build up the mesh
	shared_ptr<Mesh> resourceMesh = make_shared<Mesh>();
	for (unsigned int sm = 0; sm < scene->mNumMeshes; sm++)
	{
		aiMesh* subMesh = scene->mMeshes[sm];
		unsigned int numVertices = subMesh->mNumVertices;
		bool hasNormals = subMesh->HasNormals();
		bool hasTexCoords = subMesh->HasTextureCoords(0);
		if (numVertices == 0)
		{
			return nullptr;
		}
		// Build up our vertex structure
		aiVector3D* subMeshVertices = subMesh->mVertices;
		aiVector3D* subMeshNormals = subMesh->mNormals;
		// We only handle one set of UV coordinates at the moment.  Again, handling multiple sets of UV
		// coordinates is a future enhancement.
		aiVector3D* subMeshTexCoords = subMesh->mTextureCoords[0];
		Vertex* modelVertices = new Vertex[numVertices];
		Vertex* currentVertex = modelVertices;
		for (unsigned int i = 0; i < numVertices; i++)
		{
			currentVertex->Position = Vector3(subMeshVertices->x, subMeshVertices->y, subMeshVertices->z);
			if (hasNormals)
			{
				currentVertex->Normal = Vector3(subMeshNormals->x, subMeshNormals->y, subMeshNormals->z);
			}
			else
			{
				currentVertex->Normal = Vector3(0, 0, 0);
			}
			subMeshVertices++;
			subMeshNormals++;
			if (!hasTexCoords)
			{
				// If the model does not have texture coordinates, set them to 0
				currentVertex->TexCoord = Vector2(0.0f, 0.0f);
			}
			else
			{
				// Handle negative texture coordinates by wrapping them to positive.  This should
				// ideally be handled in the shader.  Note we are assuming that negative coordinates
				// here are no smaller than -1.0 - this may not be a valid assumption.
				if (subMeshTexCoords->x < 0)
				{
					currentVertex->TexCoord.x = abs(subMeshTexCoords->x);
				}
				else
				{
					currentVertex->TexCoord.x = subMeshTexCoords->x;
				}
				if (subMeshTexCoords->y < 0)
				{
					currentVertex->TexCoord.y = abs(subMeshTexCoords->y);
				}
				else
				{
					currentVertex->TexCoord.y = subMeshTexCoords->y;
				}
				subMeshTexCoords++;
			}
			currentVertex++;
		}

		D3D11_BUFFER_DESC vertexBufferDescriptor;
		vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * numVertices;
		vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDescriptor.CPUAccessFlags = 0;
		vertexBufferDescriptor.MiscFlags = 0;
		vertexBufferDescriptor.StructureByteStride = 0;

		// Now set up a structure that tells DirectX where to get the
		// data for the vertices from
		D3D11_SUBRESOURCE_DATA vertexInitialisationData;
		vertexInitialisationData.pSysMem = modelVertices;

		// and create the vertex buffer
		if (FAILED(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, vertexBuffer.GetAddressOf())))
		{
			return nullptr;
		}

		// Now extract the indices from the file
		unsigned int numberOfFaces = subMesh->mNumFaces;
		unsigned int numberOfIndices = numberOfFaces * 3;
		aiFace* subMeshFaces = subMesh->mFaces;
		if (subMeshFaces->mNumIndices != 3)
		{
			// We are not dealing with triangles, so we cannot handle it
			return nullptr;
		}
		unsigned int* modelIndices = new unsigned int[numberOfIndices * 3];
		unsigned int* currentIndex = modelIndices;
		for (unsigned int i = 0; i < numberOfFaces; i++)
		{
			*currentIndex++ = subMeshFaces->mIndices[0];
			*currentIndex++ = subMeshFaces->mIndices[1];
			*currentIndex++ = subMeshFaces->mIndices[2];
			subMeshFaces++;
		}
		// Setup the structure that specifies how big the index 
		// buffer should be
		D3D11_BUFFER_DESC indexBufferDescriptor;
		indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDescriptor.ByteWidth = sizeof(UINT) * numberOfIndices * 3;
		indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescriptor.CPUAccessFlags = 0;
		indexBufferDescriptor.MiscFlags = 0;
		indexBufferDescriptor.StructureByteStride = 0;

		// Now set up a structure that tells DirectX where to get the
		// data for the indices from
		D3D11_SUBRESOURCE_DATA indexInitialisationData;
		indexInitialisationData.pSysMem = modelIndices;

		// and create the index buffer
		if (FAILED(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, indexBuffer.GetAddressOf())))
		{
			return nullptr;
		}

		// Do we have a material associated with this mesh?
		shared_ptr<Material> material = nullptr;
		if (scene->HasMaterials())
		{
			material = GetMaterial(materials[subMesh->mMaterialIndex]);
		}
		shared_ptr<SubMesh> resourceSubMesh = make_shared<SubMesh>(vertexBuffer, indexBuffer, numVertices, numberOfIndices, material, hasNormals, hasTexCoords);
		resourceMesh->AddSubMesh(resourceSubMesh);
		delete[] modelVertices;
		delete[] modelIndices;
	}
	return resourceMesh;
}
