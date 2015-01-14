#include "RigidBodyComponent.h"
#include "IScene.h"
#include "GameObject.h"
#include "PhysXDataManager.h"

#include <stdexcept>
#include "PxPhysics.h"
#include "PxRigidStatic.h"
#include "PxRigidDynamic.h"
#include "PxScene.h"
#include "PxMaterial.h"
#include "geometry/PxConvexMesh.h"

#ifdef _DEBUG
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
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
	if(physxDebugNode)
	{
		Ogre::SceneManager* sceneManager = owningGameObject->GetOwningScene()->GetOgreSceneManager();
		sceneManager->destroySceneNode(physxDebugNode);
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

bool RigidBodyComponent::CreateAndAttachNewShape(const ShapeDefinition& shapeDefinition)
{
	if(shapeDefinition.MaterialList.size() == 0)
		return false;

	physx::PxShape* shape = NULL;

	if(bodyType == STATIC)
		shape =	bodyStorage.staticActor->createShape(*shapeDefinition.ShapeGeometry.get(), 
		&shapeDefinition.MaterialList[0], 
			shapeDefinition.MaterialList.size(), shapeDefinition.ShapeFlags);

	else if(bodyType == DYNAMIC)
		shape = bodyStorage.dynamicActor->createShape(*shapeDefinition.ShapeGeometry.get(), 
		&shapeDefinition.MaterialList[0],
			shapeDefinition.MaterialList.size(), shapeDefinition.ShapeFlags);

	if(shape)
		shape->setLocalPose(shapeDefinition.Transform);

	return shape != NULL;
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

void RigidBodyComponent::SetPosition(physx::PxVec3& position)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->setGlobalPose(physx::PxTransform(position, bodyStorage.dynamicActor->getGlobalPose().q));
	else if(bodyType == STATIC)
		bodyStorage.staticActor->setGlobalPose(physx::PxTransform(position, bodyStorage.staticActor->getGlobalPose().q));

#if _DEBUG
	if(physxDebugNode)
		physxDebugNode->setPosition(position.x, position.y, position.z);
#endif
}


#if _DEBUG

void RigidBodyComponent::CreateDebugDraw()
{
	if(physxDebugNode)
		return;

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

	physxDebugNode = owningGameObject->GetOwningScene()->GetOgreSceneManager()->getRootSceneNode()->createChildSceneNode();
	physxDebugNode->setInheritScale(false);

	PxU32 numShapes = rigidBody->getNbShapes();

	PxShape** shapes = new physx::PxShape*[numShapes];

	rigidBody->getShapes(shapes, numShapes);

	Ogre::SceneManager* sceneManager = owningGameObject->GetOwningScene()->GetOgreSceneManager();

	for (PxU32 i = 0; i < numShapes; i++)
	{
		switch (shapes[i]->getGeometryType())
		{
			case::PxGeometryType::eBOX:
				{
					PxBoxGeometry boxGeometry;
					shapes[i]->getBoxGeometry(boxGeometry);

					Ogre::Entity* entity = sceneManager->createEntity("Cube.mesh");
					entity->setMaterialName("WireframeRender");

					physxDebugNode->attachObject(entity);
					
					PxVec3 halfSize = boxGeometry.halfExtents;
					physxDebugNode->scale(halfSize.x, halfSize.y, halfSize.z);
				}
				
				break;

			case::PxGeometryType::eSPHERE:
				{
					PxSphereGeometry sphereGeometry;
					shapes[i]->getSphereGeometry(sphereGeometry);

					Ogre::Entity* entity = sceneManager->createEntity("Sphere.mesh");
					entity->setMaterialName("WireframeRender");

					physxDebugNode->attachObject(entity);
					physxDebugNode->scale(sphereGeometry.radius, sphereGeometry.radius, sphereGeometry.radius);
				}
				break;

			case::PxGeometryType::eCONVEXMESH:
				{
					PxConvexMeshGeometry meshGeometry;
					shapes[i]->getConvexMeshGeometry(meshGeometry);
				
					const physx::PxVec3* vertexBuffer = meshGeometry.convexMesh->getVertices();
					const physx::PxU8* indexBuffer = meshGeometry.convexMesh->getIndexBuffer();
									
					Ogre::ManualObject* manObject = sceneManager->createManualObject();
					manObject->begin("WireframeRender", Ogre::RenderOperation::OT_TRIANGLE_LIST);
					
					for (unsigned int i = 0; i < meshGeometry.convexMesh->getNbVertices(); i++)
					{
						manObject->position(vertexBuffer[i].x,vertexBuffer[i].y, vertexBuffer[i].z );
					}

					for (unsigned int i = 0; i < meshGeometry.convexMesh->getNbPolygons(); i++)
					{
						physx::PxHullPolygon poly;
						if(meshGeometry.convexMesh->getPolygonData(i, poly))
						{
							manObject->quad(indexBuffer[poly.mIndexBase],
								indexBuffer[poly.mIndexBase + 1], 
								indexBuffer[poly.mIndexBase + 2], 
								indexBuffer[poly.mIndexBase + 3]);
						}
					}

					manObject->end();
					
					physxDebugNode->attachObject(manObject);
				}
			default:
				break;
		}
	}
	
	if(shapes)
		delete shapes;

	
	// add ManualObject to the node so it will be visible
	//physxDebugNode->attachObject(physxDebugDraw);
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