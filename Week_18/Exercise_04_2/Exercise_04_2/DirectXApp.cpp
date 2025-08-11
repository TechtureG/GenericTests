#include "DirectXApp.h"
#include "CubeNode.h"


DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// Add your code here to build up the scene graph

	// Create a cube node using your full lighting, shader, and triangle setup
	SceneNodePointer cube = make_shared<CubeNode>(L"CubeNode");

	// Add the cube to the scene
	sceneGraph->Add(cube);

}

void DirectXApp::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This method is called at the frame rate frequency set in the Framework class (by default,
	// 60 times a second).  Perform any updates to the scene graph needed for the next frame. 
}
