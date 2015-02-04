#include "Probender.h"
#include "IScene.h"
#include "InputNotifier.h"
#include "NotImplementedException.h"
#include "PhysXDataManager.h"
#include "Arena.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"

#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "ProbenderData.h"
#include "PxPhysics.h"
#include "extensions\PxSimpleFactory.h"
#include "PxMaterial.h"

#include "OgreRoot.h"
#include "OgreSceneManager.h"
#include "OgreManualObject.h"
#include "OgreSubMesh.h"
#include "OgreMeshManager.h"
#include "OgreHardwareBufferManager.h"

Probender::Probender()
	: GameObject(NULL), leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL)
{
}

Probender::Probender(const unsigned short _contestantID, const ContestantColour _colour, Arena* _owningArena)
	: GameObject(_owningArena->GetOwningScene(), "Probender" + _contestantID), contestantID(_contestantID), owningArena(_owningArena), 
		leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL), colour(_colour)
{
	
}

Probender::~Probender(void)
{
}

void Probender::Start()
{
	SetInputState(Probender::Listen);
	inputHandler.SetProbenderToHandle(this);

	MeshRenderComponent* renderComponent = new MeshRenderComponent();
	AttachComponent(renderComponent);
	
	std::string entityToLoad = GetMeshAndMaterialName();

	//Try loading required model
	renderComponent->LoadModel(entityToLoad);
	renderComponent->SetMaterial(entityToLoad);
	
	meshRenderComponent = renderComponent;

	RigidBodyComponent* rigid = new RigidBodyComponent();
	AttachComponent(rigid);

	rigid->CreateRigidBody(RigidBodyComponent::DYNAMIC);

	ShapeDefinition shapeDef = ShapeDefinition();
	shapeDef.SetBoxGeometry(physx::PxVec3(0.250f, 0.60f, 0.050f));
	shapeDef.AddMaterial("101000");
	PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, "ProbenderShape");
	rigid->AttachShape("ProbenderShape");
	rigid->SetKinematic(true);

	rigid->CreateDebugDraw();
	
	GameObject::Start();
}

void Probender::Update(float gameTime)
{
	GameObject::Update(gameTime);

	inputHandler.Update(gameTime);
	stateManager.Update(gameTime);	
}

void Probender::AcquireNewTarget(bool toRight)
{
	throw NotImplementedException();
}

void Probender::CreateInGameData(const ProbenderData& data)
{
	characterData.ElementAbilities.Element = data.Attributes.MainElement;
	characterData.SubelementAbilities.Element = data.Attributes.SubElement;

	currentElement = characterData.GetMainElement();
}

void Probender::SetInputState(const InputState newState)
{
	switch (newState)
	{
	case Probender::Listen:
		inputHandler.BeginListeningToAll();
		break;
	case Probender::Pause:
		inputHandler.PauseListeningToAll();
		break;
	case Probender::Stop:
		inputHandler.StopListeningToAll();
		break;
	}
}

void Probender::SetCurrentElement(const ElementEnum::Element elementToSet)
{
	if(currentElement != elementToSet)
	{
		//If the element is one of the elements available to the bender
		/*if(elementToSet == characterData.GetMainElement() ||
		elementToSet == characterData.GetSubElement())*/
		{
			currentElement = elementToSet;
		}
	}
}

void Probender::RemoveProjectile(SharedProjectile projectileToRemove)
{
	//Check both hands, in case its a two hand attack
	if(leftHandAttack == projectileToRemove)
		leftHandAttack.reset();
	
	if(rightHandAttack == projectileToRemove)
		rightHandAttack.reset();
}

void Probender::CreateContestantMeshes(Ogre::SceneManager* sceneMan, bool red,
			bool blue, bool green, bool yellow, bool purple, bool orange)
{
	Ogre::ManualObject* manObject = sceneMan->createManualObject();

	manObject->begin("RedProbender", Ogre::RenderOperation::OT_LINE_LIST);

	//Set all to 0, because ProbenderInput will update them based on Kinect Data
	for (int i = 0; i < RenderableJointType::Count; i++)
	{
		manObject->position(0.0f, 0.0f, 0.0f);
	}

	manObject->index(RenderableJointType::FootLeft); manObject->index(RenderableJointType::AnkleLeft);
			manObject->index(RenderableJointType::AnkleLeft); manObject->index(RenderableJointType::KneeLeft);
			manObject->index(RenderableJointType::KneeLeft); manObject->index(RenderableJointType::HipLeft);
			manObject->index(RenderableJointType::HipLeft); manObject->index(RenderableJointType::SpineBase);
			//Lower Right Body
			manObject->index(RenderableJointType::FootRight); manObject->index(RenderableJointType::AnkleRight);
			manObject->index(RenderableJointType::AnkleRight); manObject->index(RenderableJointType::KneeRight);
			manObject->index(RenderableJointType::KneeRight); manObject->index(RenderableJointType::HipRight);
			manObject->index(RenderableJointType::HipRight); manObject->index(RenderableJointType::SpineBase);
			//Torso and Head
			manObject->index(RenderableJointType::SpineBase); manObject->index(RenderableJointType::SpineMid);
			manObject->index(RenderableJointType::SpineMid); manObject->index(RenderableJointType::SpineShoulder);
			manObject->index(RenderableJointType::SpineShoulder); manObject->index(RenderableJointType::Neck);
			manObject->index(RenderableJointType::Neck); manObject->index(RenderableJointType::Head);
			//Left Arm
			manObject->index(RenderableJointType::SpineShoulder); manObject->index(RenderableJointType::ShoulderLeft);
			manObject->index(RenderableJointType::ShoulderLeft); manObject->index(RenderableJointType::ElbowLeft);
			manObject->index(RenderableJointType::ElbowLeft); manObject->index(RenderableJointType::WristLeft);
			manObject->index(RenderableJointType::WristLeft); manObject->index(RenderableJointType::HandLeft);
			manObject->index(RenderableJointType::HandLeft); manObject->index(RenderableJointType::ThumbLeft);
			manObject->index(RenderableJointType::HandLeft); manObject->index(RenderableJointType::HandTipLeft);
			//Right Arm
			manObject->index(RenderableJointType::SpineShoulder); manObject->index(RenderableJointType::ShoulderRight);
			manObject->index(RenderableJointType::ShoulderRight); manObject->index(RenderableJointType::ElbowRight);
			manObject->index(RenderableJointType::ElbowRight); manObject->index(RenderableJointType::WristRight);
			manObject->index(RenderableJointType::WristRight); manObject->index(RenderableJointType::HandRight);
			manObject->index(RenderableJointType::HandRight); manObject->index(RenderableJointType::ThumbRight);
			manObject->index(RenderableJointType::HandRight); manObject->index(RenderableJointType::HandTipRight);

	manObject->end();

	if(red)
		manObject->convertToMesh("RedProbender");
	
	if(blue)
		manObject->convertToMesh("BlueProbender");

	if(green)
		manObject->convertToMesh("GreenProbender");

	if(yellow)
		manObject->convertToMesh("YellowProbender");

	if(orange)
		manObject->convertToMesh("OrangeProbender");

	if(purple)
		manObject->convertToMesh("PurpleProbender");
	//Ogre::MeshPtr contestantMesh = Ogre::MeshManager::getSingletonPtr()->createManual("RedProbender", "General");

	///// Create one submesh
	//Ogre::SubMesh* sub = contestantMesh->createSubMesh();
	//	Ogre::RenderOperation op;
	//	sub->operationType = Ogre::RenderOperation::OT_LINE_LIST;
	//	sub->_getRenderOperation(op);

	//	op.operationType = Ogre::RenderOperation::OT_LINE_LIST;

	//	sub->_getRenderOperation(op);

	//const float sqrt13 = 0.577350269f; /* sqrt(1/3) */

	///// Define the vertices 
	//const size_t nVertices = RenderableJointType::Count;
	//const size_t vbufCount = 3*nVertices;

	//float vertices[vbufCount] = {
	//	0.0f, 0.0f, 0.0f,        //0 position
	//	0.0f, 0.0f, 0.0f,         //1 position
	//	0.0f, 0.0f, 0.0f,        //2 position
	//	0.0f, 0.0f, 0.0f,       //3 position
	//	0.0f, 0.0f, 0.0f,         //4 position
	//	0.0f, 0.0f, 0.0f,          //5 position
	//	0.0f, 0.0f, 0.0f,         //6 position
	//	0.0f, 0.0f, 0.0f,        //7 position
	//	0.0f, 0.0f, 0.0f,        //8 position
	//	0.0f, 0.0f, 0.0f,        //9 position
	//	0.0f, 0.0f, 0.0f,        //10 position
	//	0.0f, 0.0f, 0.0f,        //11 position
	//	0.0f, 0.0f, 0.0f,        //12 position
	//	0.0f, 0.0f, 0.0f,        //13 position
	//	0.0f, 0.0f, 0.0f,        //14 position
	//	0.0f, 0.0f, 0.0f,        //15 position
	//	0.0f, 0.0f, 0.0f,        //16 position
	//	0.0f, 0.0f, 0.0f,        //17 position
	//	0.0f, 0.0f, 0.0f,        //18 position
	//	0.0f, 0.0f, 0.0f,        //19 position
	//	0.0f, 0.0f, 0.0f,        //20 position
	//	0.0f, 0.0f, 0.0f,        //21 position
	//	0.0f, 0.0f, 0.0f,        //22 position
	//	0.0f, 0.0f, 0.0f,        //23 position
	//	0.0f, 0.0f, 0.0f        //24 position
	//};

	///// Define 12 triangles (two triangles per cube face)
	///// The values in this table refer to vertices in the above table
	//const size_t ibufCount = 48;
	//unsigned short lines[ibufCount] = {
	//	//Lower Left Body
	//	RenderableJointType::FootLeft, RenderableJointType::AnkleLeft,
	//	RenderableJointType::AnkleLeft, RenderableJointType::KneeLeft,
	//	RenderableJointType::KneeLeft, RenderableJointType::HipLeft,
	//	RenderableJointType::HipLeft, RenderableJointType::SpineBase,
	//	//Lower Right Body
	//	RenderableJointType::FootRight, RenderableJointType::AnkleRight,
	//	RenderableJointType::AnkleRight, RenderableJointType::KneeRight,
	//	RenderableJointType::KneeRight, RenderableJointType::HipRight,
	//	RenderableJointType::HipRight, RenderableJointType::SpineBase,
	//	//Torso and Head
	//	RenderableJointType::SpineBase, RenderableJointType::SpineMid,
	//	RenderableJointType::SpineMid, RenderableJointType::SpineShoulder,
	//	RenderableJointType::SpineShoulder, RenderableJointType::Neck,
	//	RenderableJointType::Neck, RenderableJointType::Head,
	//	//Left Arm
	//	RenderableJointType::SpineShoulder, RenderableJointType::ShoulderLeft,
	//	RenderableJointType::ShoulderLeft, RenderableJointType::ElbowLeft,
	//	RenderableJointType::ElbowLeft, RenderableJointType::WristLeft,
	//	RenderableJointType::WristLeft, RenderableJointType::HandLeft,
	//	RenderableJointType::HandLeft, RenderableJointType::ThumbLeft,
	//	RenderableJointType::HandLeft, RenderableJointType::HandTipLeft,
	//	//Right Arm
	//	RenderableJointType::SpineShoulder, RenderableJointType::ShoulderRight,
	//	RenderableJointType::ShoulderRight, RenderableJointType::ElbowRight,
	//	RenderableJointType::ElbowRight, RenderableJointType::WristRight,
	//	RenderableJointType::WristRight, RenderableJointType::HandRight,
	//	RenderableJointType::HandRight, RenderableJointType::ThumbRight,
	//	RenderableJointType::HandRight, RenderableJointType::HandTipRight,
	//};

	///// Create vertex data structure for 8 vertices shared between submeshes
	//contestantMesh->sharedVertexData = new Ogre::VertexData();
	//contestantMesh->sharedVertexData->vertexCount = nVertices;
	//
	///// Create declaration (memory format) of vertex data
	//Ogre::VertexDeclaration* decl = contestantMesh->sharedVertexData->vertexDeclaration;
	//size_t offset = 0;
	//// 1st buffer
	//decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);

	///// Allocate vertex buffer of the requested number of vertices (vertexCount) 
	///// and bytes per vertex (offset)
	//Ogre::HardwareVertexBufferSharedPtr vbuf = 
	//	Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
	//	offset, contestantMesh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	///// Upload the vertex data to the card
	//vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

	///// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
	//Ogre::VertexBufferBinding* bind = contestantMesh->sharedVertexData->vertexBufferBinding; 
	//bind->setBinding(0, vbuf);

	///// Allocate index buffer of the requested number of vertices (ibufCount) 
	//Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
	//	createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, 
	//	ibufCount, 
	//	Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	///// Upload the index data to the card
	//ibuf->writeData(0, ibuf->getSizeInBytes(), lines, true);

	///// Set parameters of the submesh
	//sub->useSharedVertices = true;
	//sub->indexData->indexBuffer = ibuf;
	//sub->indexData->indexCount = ibufCount;
	//sub->indexData->indexStart = 0;

	///// Set bounding information (for culling)
	//contestantMesh->_setBounds(Ogre::AxisAlignedBox(-0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f));
	//contestantMesh->_setBoundingSphereRadius(Ogre::Math::Sqrt(3*1.0f));

	///// Notify -Mesh object that it has been loaded
	//contestantMesh->load();

}

std::string Probender::GetMeshAndMaterialName()
{
	switch (colour)
	{
	case BLUE:
		return "BlueProbender";
		break;

	case GREEN:
		return "GreenProbender";
		break;

	case ORANGE:
		return "OrangeProbender";
		break;

	case PURPLE:
		return "PurpleProbender";
		break;

	case RED:
		return "RedProbender";
		break;

	case YELLOW:
		return "YellowProbender";
		break;
	default:
		return "";
		break;
	}
}
