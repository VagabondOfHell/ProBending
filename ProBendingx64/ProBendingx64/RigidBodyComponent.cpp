#include "RigidBodyComponent.h"
#include "IScene.h"
#include "GameObject.h"

#include <stdexcept>
#include "PxPhysics.h"
#include "PxRigidStatic.h"
#include "PxRigidDynamic.h"
#include "PxScene.h"

#ifdef _DEBUG
#include "OgreSceneManager.h"
#include "PhysXDebugDraw.h"
#endif

using namespace physx;

RigidBodyComponent::RigidBodyComponent()
	:Component(), bodyType(NONE)
{
	bodyStorage.staticActor = NULL; //set one of the union to NULL to reset whole structure
}

RigidBodyComponent::~RigidBodyComponent(void)
{
	if(bodyType == DYNAMIC)
		if(bodyStorage.dynamicActor)
			bodyStorage.dynamicActor->release();

	if(bodyType == STATIC)
		if(bodyStorage.staticActor)
			bodyStorage.staticActor->release();

#ifdef _DEBUG
	if(physxDebugDraw)
	{
		physxDebugNode->detachObject(physxDebugDraw);
		Ogre::SceneManager* sceneManager = owningGameObject->GetOwningScene()->GetOgreSceneManager();
		sceneManager->destroyManualObject(physxDebugDraw);
		sceneManager->destroySceneNode(physxDebugNode);
		physxDebugDraw = NULL;
	}
#endif
}

bool RigidBodyComponent::CreateRigidBody(RigidBodyType _bodyType, physx::PxVec3& position 
				/*= physx::PxVec3(0.0f)*/, physx::PxQuat& orientation /*= physx::PxQuat::createIdentity()*/)
{	
	if(bodyType != NONE)
		return false;

	//Create appropriate shape
	switch (_bodyType)
	{
	case RigidBodyComponent::NONE:
		return false;
		break;
	case RigidBodyComponent::STATIC:
		bodyStorage.staticActor = PxGetPhysics().createRigidStatic(PxTransform(position, orientation));
		break;
	case RigidBodyComponent::DYNAMIC:
		bodyStorage.dynamicActor = PxGetPhysics().createRigidDynamic(PxTransform(position, orientation));
		break;
	default:
		break;
	}

	bodyType = _bodyType;

	return (bodyStorage.staticActor || bodyStorage.dynamicActor); //Check if one of the union has been initialized
}

bool RigidBodyComponent::AttachShape(PxShape& newShape)
{
	switch (bodyType)
	{
	case RigidBodyComponent::NONE:
		return false;
		break;
	case RigidBodyComponent::STATIC:
		bodyStorage.staticActor->attachShape(newShape);
		break;
	case RigidBodyComponent::DYNAMIC:
		bodyStorage.dynamicActor->attachShape(newShape);
		break;
	default:
		return false;
		break;
	}

	return true;
}

bool RigidBodyComponent::AttachShape(PxGeometry& geometry, PxMaterial& material, PxTransform transform)
{
	if(bodyType == NONE)
		return false;

	PxShape* shape = PxGetPhysics().createShape(geometry, material);

	if(shape)
		shape->setLocalPose(transform);

	return AttachShape(*shape);
}

bool RigidBodyComponent::AttachShape(PxGeometry& geometry, PxReal staticFriction /*= 0.0f*/, 
		PxReal dynamicFriction /*= 0.0f*/, PxReal restitution /*= 0.0f*/, 
		PxVec3& position /*= PxVec3(0.0f)*/, PxQuat& orientation /*= PxQuat::createIdentity()*/)
{
	if(bodyType == NONE)
		return false;

	PxMaterial* material = PxGetPhysics().createMaterial(staticFriction, dynamicFriction, restitution);
	
	if(material == NULL)
		return false;

	return AttachShape(geometry, *material, PxTransform(position, orientation));
}

void RigidBodyComponent::SetUseGravity(const bool val)
{
	switch (bodyType)
	{
	case RigidBodyComponent::NONE:
		return;
		break;
	case RigidBodyComponent::STATIC:
		bodyStorage.staticActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !val);//use the opposite of the val to decide to disable gravity
		break;
	case RigidBodyComponent::DYNAMIC:
		bodyStorage.dynamicActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !val);//use the opposite of the val to decide to disable gravity
		break;
	default:
		break;
	}
}

void RigidBodyComponent::ApplyImpulse(physx::PxVec3& impulse)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->addForce(impulse, physx::PxForceMode::eIMPULSE);
}

void RigidBodyComponent::ApplyForce(physx::PxVec3& force)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->addForce(force, physx::PxForceMode::eFORCE);
}


#if _DEBUG

void RigidBodyComponent::CreateDebugDraw()
{
	PxRigidActor* rigidBody = NULL;
	
	switch (bodyType)
	{
	case RigidBodyComponent::NONE:
		return;
		break;
	case RigidBodyComponent::STATIC:
		rigidBody = bodyStorage.staticActor;
		break;
	case RigidBodyComponent::DYNAMIC:
		rigidBody = bodyStorage.dynamicActor;
		break;
	}

	physxDebugDraw = owningGameObject->GetOwningScene()->GetOgreSceneManager()->createManualObject();

	PxU32 numShapes = rigidBody->getNbShapes();

	PxShape** shapes = new physx::PxShape*[numShapes];

	rigidBody->getShapes(shapes, numShapes);

	PxBoxGeometry boxGeometry;
		
	for (PxU32 i = 0; i < numShapes; i++)
	{
		switch (shapes[i]->getGeometryType())
		{
			case::PxGeometryType::eBOX:
				shapes[i]->getBoxGeometry(boxGeometry);

			PhysXDebugDraw::DrawBoxGeometry(rigidBody->getGlobalPose().p + shapes[i]->getLocalPose().p, &boxGeometry, physxDebugDraw);
				break;

			default:
				break;
		}
	}
	
	if(shapes)
		delete shapes;

	physxDebugNode = owningGameObject->GetOwningScene()->GetOgreSceneManager()->getRootSceneNode()->createChildSceneNode();
	// add ManualObject to the node so it will be visible
	physxDebugNode->attachObject(physxDebugDraw);
}


#endif

void RigidBodyComponent::Start()
{
	//Add the rigid body to the scene
	switch (bodyType)
	{
	case RigidBodyComponent::NONE:
		break;
	case RigidBodyComponent::STATIC:
		owningGameObject->GetOwningScene()->GetPhysXScene()->addActor(*bodyStorage.staticActor);
		break;
	case RigidBodyComponent::DYNAMIC:
		owningGameObject->GetOwningScene()->GetPhysXScene()->addActor(*bodyStorage.dynamicActor);
		break;
	default:
		break;
	}
}

void RigidBodyComponent::Update(float gameTime)
{
	if(bodyType == DYNAMIC)
	{
		PxTransform globalPose = bodyStorage.dynamicActor->getGlobalPose();
		owningGameObject->SetWorldPosition(globalPose.p.x, globalPose.p.y, globalPose.p.z);
		owningGameObject->SetWorldOrientation(globalPose.q.w, globalPose.q.x, globalPose.q.y, globalPose.q.z);

#if _DEBUG
		if(physxDebugNode)
		{
			physxDebugNode->_setDerivedPosition(Ogre::Vector3(globalPose.p.x, globalPose.p.y, globalPose.p.z));
			physxDebugNode->_setDerivedOrientation(Ogre::Quaternion(globalPose.q.w, globalPose.q.x, globalPose.q.y, globalPose.q.z));
		}
#endif
	}
}


