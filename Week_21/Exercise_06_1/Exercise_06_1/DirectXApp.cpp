#include "DirectXApp.h"
#include "TerrainNode.h"


DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// Add your code here to build up the scene graph

	SceneNodePointer terrainNode_ptr = make_shared<TerrainNode>(L"Terrain");
	terrainNode_ptr->Initialise();
	sceneGraph->Add(terrainNode_ptr);

}

void DirectXApp::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This method is called at the frame rate frequency set in the Framework class (by default,
	// 60 times a second).  Perform any updates to the scene graph needed for the next frame. 
}
