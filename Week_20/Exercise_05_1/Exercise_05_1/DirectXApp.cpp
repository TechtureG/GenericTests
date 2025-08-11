#include "DirectXApp.h"
#include "MeshNode.h"


DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// Add your code here to build up the scene graph

	// Create a plane road from a model file
	SceneNodePointer  nodePlane = make_shared<MeshNode>(L"Plane", L".\\BiPlane\\airplane.x");
	nodePlane->Initialise();
	// Add the plane to the scene
	sceneGraph->Add(nodePlane);

	//Apply an initial translation
	Matrix ModelTrans = Matrix::CreateTranslation(-10.0f, 0.0f, 0.0f);
	sceneGraph->Find(L"Plane")->SetWorldTransform(ModelTrans);

}

void DirectXApp::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This method is called at the frame rate frequency set in the Framework class (by default,
	// 60 times a second).  Perform any updates to the scene graph needed for the next frame. 

	// Update our model's world transformations
	float rotationAngle = -0.5f;
	//Compute the composite Transformation for Plane
	Matrix ModelYRot = Matrix::CreateRotationY(rotationAngle * XM_PI / 180.0f);
	
	Matrix worldTransformation = sceneGraph->Find(L"Plane")->GetWorldTransform();
	worldTransformation = worldTransformation * ModelYRot;
	
	sceneGraph->Find(L"Plane")->SetWorldTransform(worldTransformation);
	
}
