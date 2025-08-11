#include "DirectXApp.h"
#include "TerrainNode.h"
#include "Camera.h"


DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// Add your code here to build up the scene graph

	SceneNodePointer terrainNode_ptr = make_shared<TerrainNode>(L"Terrain");
	terrainNode_ptr->Initialise();
	sceneGraph->Add(terrainNode_ptr);


	GetCamera()->SetCameraPosition(0.0f, 50.0f, -300.0f);

}

void DirectXApp::UpdateCamera()
{
	float fwdbwdStep = 1.0f;
	float angleStep = 0.5f;

	if ((GetAsyncKeyState('W') < 0) && !(GetAsyncKeyState(VK_SHIFT) < 0))
	{
		GetCamera()->SetForwardBack(fwdbwdStep);
	}
	if ((GetAsyncKeyState('S') < 0) && !(GetAsyncKeyState(VK_SHIFT) < 0))
	{
		GetCamera()->SetForwardBack(-fwdbwdStep);
	}
	if (GetAsyncKeyState('A') < 0)
	{
		GetCamera()->SetYaw(-angleStep);
	}
	if (GetAsyncKeyState('D') < 0)
	{
		GetCamera()->SetYaw(angleStep);
	}
	if ((GetAsyncKeyState('W') < 0) && (GetAsyncKeyState(VK_SHIFT) < 0))
	{
		GetCamera()->SetPitch(-angleStep);
	}
	if ((GetAsyncKeyState('S') < 0) && (GetAsyncKeyState(VK_SHIFT) < 0))
	{
		GetCamera()->SetPitch(angleStep);
	}
	if (GetAsyncKeyState('Q') < 0)
	{
		GetCamera()->SetRoll(-angleStep);
	}
	if (GetAsyncKeyState('E') < 0)
	{
		GetCamera()->SetRoll(angleStep);
	}

	GetCamera()->Update();
}

void DirectXApp::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This method is called at the frame rate frequency set in the Framework class (by default,
	// 60 times a second).  Perform any updates to the scene graph needed for the next frame. 

	UpdateCamera();
}
