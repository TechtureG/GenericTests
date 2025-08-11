#include "SceneGraph.h"

//Initialise Method
bool SceneGraph::Initialise(void)
{
	for (size_t i = 0; i < _children.size(); i++)
	{
		if (!(_children.at(i)->Initialise()))
		{
			return false;
		}
	}
	return true;
}

//Update Method
void SceneGraph::Update(const Matrix& currentWorldTransformation)
{
	SceneNode::Update(currentWorldTransformation);
	//XMMATRIX combinedWorldTransformation = XMLoadFloat4x4(&_worldTransformation);

	for (size_t i = 0; i < _children.size(); i++)
	{
		_children.at(i)->Update(currentWorldTransformation);
	}
}

//Render Method
void SceneGraph::Render(void)
{
	for (size_t i = 0; i < _children.size(); i++)
	{
		_children.at(i)->Render();
	}
}

//Shutdown Method
void SceneGraph::Shutdown(void)
{
	for (size_t i = 0; i < _children.size(); i++)
	{
		_children.at(i)->Shutdown();
	}
}

//Add Method
void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

//Remove Method
void SceneGraph::Remove(SceneNodePointer node)
{
	for (size_t i = 0; i < _children.size(); i++)
	{
		if (_children.at(i) == node)
		{
			_children.erase(_children.begin() + i);
		}
	}
}

//Find Method
SceneNodePointer SceneGraph::Find(wstring name)
{
	if (name == _name)
	{
		return shared_from_this();
	}
	else
	{
		for (size_t i = 0; i < _children.size(); i++)
		{
			if (_children.at(i)->Find(name) != nullptr)
			{
				return _children.at(i)->Find(name);
			}
		}
	}
	return nullptr;
}
