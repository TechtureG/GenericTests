#include "DirectXApp.h"
#include "MeshNode.h"


DirectXApp app;

void DirectXApp::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// Add your code here to build up the scene graph

	// Create a plane node from a model file
	SceneNodePointer  nodePlane = make_shared<MeshNode>(L"Plane", L".\\BiPlane_SeparateProp\\airplane.x");
	nodePlane->Initialise();
	// Add the plane to the scene
	sceneGraph->Add(nodePlane);

	// Create a propeller node from a model file
	SceneNodePointer  nodePropeller = make_shared<MeshNode>(L"Propeller", L".\\BiPlane_SeparateProp\\prop.x");
	nodePropeller->Initialise();
	// Add the plane to the scene
	sceneGraph->Add(nodePropeller);

	//Apply an initial translation
	Matrix ModelTrans = Matrix::CreateTranslation(-10.0f, 0.0f, 0.0f);
	sceneGraph->Find(L"Plane")->SetWorldTransform(ModelTrans);
	sceneGraph->Find(L"Propeller")->SetWorldTransform(ModelTrans);

}

void DirectXApp::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();

	// This method is called at the frame rate frequency set in the Framework class (by default,
	// 60 times a second).  Perform any updates to the scene graph needed for the next frame. 

	// Update our model's world transformations
	float rotationAngle = -0.75f;
	//Compute the composite Transformation for Plane
	Matrix worldTransformation, ModelYRot, ModelZRot;
	SceneNodePointer Model;
	
	ModelYRot = Matrix::CreateRotationY(rotationAngle * XM_PI / 180.0f);
	Model = sceneGraph->Find(L"Plane");
	worldTransformation = Model->GetWorldTransform();
	worldTransformation = worldTransformation * ModelYRot;
	
	Model->SetWorldTransform(worldTransformation);

	ModelZRot = Matrix::CreateTranslation(-0.03f, -0.42f, 0.0f)*Matrix::CreateRotationZ(30.0f * rotationAngle * XM_PI / 180.0f)*Matrix::CreateTranslation(+0.03f, +0.42f, 0.0f);
	//ModelZRot = Matrix::CreateRotationZ(0.0f * rotationAngle * XM_PI / 180.0f);
	Model = sceneGraph->Find(L"Propeller");
	worldTransformation = Model->GetWorldTransform();
	worldTransformation = ModelZRot * worldTransformation * ModelYRot;

	Model->SetWorldTransform(worldTransformation);
	
}
