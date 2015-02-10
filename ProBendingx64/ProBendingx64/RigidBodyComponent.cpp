#include "RigidBodyComponent.h"
#include "IScene.h"
#include "GameObject.h"
#include "PhysXDataManager.h"

#include <stdexcept>
#include "PxPhysics.h"
#include "extensions/PxRigidBodyExt.h"
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
#include "OgreNameGenerator.h"

using namespace physx;

RigidBodyComponent::RigidBodyComponent()
	:Component(), bodyType(NONE)
{
	forces = PxVec3(0.0f);
	velocity = PxVec3(0.0f);

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
	
	orientation.normalize();

	//Create appropriate shape
	switch (_bodyType)
	{
	case RigidBodyComponent::NONE:
		return false;
		break;
	case RigidBodyComponent::STATIC:
		bodyStorage.staticActor = PxGetPhysics().createRigidStatic(PxTransform(position, orientation));
		bodyStorage.staticActor->userData = owningGameObject;
		break;
	case RigidBodyComponent::DYNAMIC:
		bodyStorage.dynamicActor = PxGetPhysics().createRigidDynamic(PxTransform(position, orientation));
		bodyStorage.dynamicActor->userData = owningGameObject;
		bodyStorage.dynamicActor->setContactReportThreshold(0.0f);
		break;
	default:
		break;
	}

	bodyType = _bodyType;

	owningGameObject->SetWorldPosition(position.x, position.y, position.z);
	owningGameObject->SetWorldOrientation(orientation.w, orientation.x, orientation.y, orientation.z);

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

bool RigidBodyComponent::AttachShape(const std::string& shapeName)
{
	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->GetShape(shapeName);

	if(shape)
		return AttachShape(*shape);

	return false;
}

bool RigidBodyComponent::CreateAndAttachNewShape(const ShapeDefinition& shapeDefinition, const std::string& shapeName)
{
	if(shapeDefinition.MaterialList.size() == 0)
		return false;

	physx::PxShape* shape = PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDefinition, shapeName);

	if(shape)
	{
		if(bodyType == STATIC)
			bodyStorage.staticActor->attachShape(*shape);
		else if(bodyType == DYNAMIC)
			bodyStorage.dynamicActor->attachShape(*shape);

		shape->setLocalPose(shapeDefinition.Transform);
	}

	return shape != NULL;
}

bool RigidBodyComponent::CalculateCenterOfMass(const float bodyMass, const physx::PxVec3 centerOfMass/* = physx::PxVec3(0.0f)*/)
{
	if(bodyType != DYNAMIC)
		return false;
	
	return physx::PxRigidBodyExt::setMassAndUpdateInertia(*bodyStorage.dynamicActor, bodyMass, &centerOfMass, true);
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

void RigidBodyComponent::ApplyTorqueForce(physx::PxVec3& force)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->addTorque(force, physx::PxForceMode::eFORCE);
}

void RigidBodyComponent::ApplyTorqueImpulse(physx::PxVec3& impulse)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->addTorque(impulse, physx::PxForceMode::eIMPULSE);
}

bool RigidBodyComponent::IsKinematic()
{
	return bodyStorage.dynamicActor->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC);
}

physx::PxVec3 RigidBodyComponent::GetMassSpaceInertiaTensor() const
{
	if(bodyType == DYNAMIC)
		return bodyStorage.dynamicActor->getMassSpaceInertiaTensor();

	return physx::PxVec3(0.0f);
}

void RigidBodyComponent::SetMassSpaceInertiaTensor(const physx::PxVec3& diagonal)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->setMassSpaceInertiaTensor(diagonal);
}

void RigidBodyComponent::SetPosition(physx::PxVec3& position)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->setGlobalPose(physx::PxTransform(position, bodyStorage.dynamicActor->getGlobalPose().q));
	else if(bodyType == STATIC)
		bodyStorage.staticActor->setGlobalPose(physx::PxTransform(position, bodyStorage.staticActor->getGlobalPose().q));
}

void RigidBodyComponent::SetOrientation(physx::PxQuat& orientation)
{
	orientation.normalize();
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->setGlobalPose(physx::PxTransform(bodyStorage.dynamicActor->getGlobalPose().p, 
			orientation));
	else if(bodyType == STATIC)
		bodyStorage.staticActor->setGlobalPose(physx::PxTransform(bodyStorage.staticActor->getGlobalPose().p, 
			orientation));
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
	//GetOwningScene()->GetOgreSceneManager()->getRootSceneNode()
	physxDebugNode = owningGameObject->gameObjectNode->createChildSceneNode();
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

			case::PxGeometryType::ePLANE:
				{
					PxPlaneGeometry planeGeometry;
					shapes[i]->getPlaneGeometry(planeGeometry);
					
					Ogre::Entity* entity = sceneManager->createEntity("BasicPlane");
					entity->setMaterialName("PlaneRender");

					physxDebugNode->setScale(owningGameObject->GetWorldScale());
					physxDebugNode->attachObject(entity);
				}
				break;
			case::PxGeometryType::eCONVEXMESH:
				{
					PxConvexMeshGeometry meshGeometry;
					shapes[i]->getConvexMeshGeometry(meshGeometry);
				
					const physx::PxVec3* vertexBuffer = meshGeometry.convexMesh->getVertices();
					const physx::PxU8* indexBuffer = meshGeometry.convexMesh->getIndexBuffer();
							
					//Ogre::Vector3 scale = owningGameObject->GetWorldScale();
					physx::PxVec3 scale = meshGeometry.scale.scale;

					Ogre::ManualObject* manObject = sceneManager->createManualObject();
					manObject->begin("WireframeRender", Ogre::RenderOperation::OT_TRIANGLE_LIST);
					
					Ogre::Vector3 pos = owningGameObject->GetWorldPosition();

					for (unsigned int i = 0; i < meshGeometry.convexMesh->getNbVertices(); i++)
					{
						//manObject->position(vertexBuffer[i].x,vertexBuffer[i].y, vertexBuffer[i].z );
						manObject->position(vertexBuffer[i].x * scale.x,vertexBuffer[i].y * scale.y, vertexBuffer[i].z* scale.z );
						//manObject->position(vertexBuffer[i].x - pos.x,vertexBuffer[i].y - pos.y, vertexBuffer[i].z - pos.z );
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
										
					//Ogre::MeshPtr mesh = manObject->convertToMesh(ng.generate());
					physxDebugNode->attachObject(manObject);//sceneManager->createEntity(mesh));
					//physxDebugNode->setScale(scale);
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

		if(IsKinematic())
		{
			//if(forces != PxVec3(PxZero))
			{
				float mass = GetMass();
				PxVec3 accel = CalculateAcceleration();
				PxVec3 displacement = CalculateDisplacement(gameTime, accel);
				SetKinematicTarget(globalPose.p + displacement);
				velocity = CalculateVelocity(gameTime, accel);
			}
		}
		
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

void RigidBodyComponent::PrintRigidData()
{
	if(bodyType == NONE)
		return;

	physx::PxBaseFlags baseFlag;
	physx::PxActorFlags actorFlag;
	float angleDamping;
	physx::PxTransform cMassLocalPose;
	physx::PxType concreteType;
	physx::PxTransform globalPose;
	float linearDamping, mass, maxAngleVel;
	physx::PxVec3 massInertiaTensor;
	int numShapes;
	physx::PxRigidBodyFlags rigidFlag; 
	physx::PxActorType::Enum actorType;
	physx::PxRigidDynamicFlags dynaFlag;

	if(bodyType == DYNAMIC)
	{
		baseFlag = bodyStorage.dynamicActor->getBaseFlags();
		actorFlag = bodyStorage.dynamicActor->getActorFlags();
		angleDamping = bodyStorage.dynamicActor->getAngularDamping();
		cMassLocalPose = bodyStorage.dynamicActor->getCMassLocalPose();
		concreteType = bodyStorage.dynamicActor->getConcreteType();
		globalPose = bodyStorage.dynamicActor->getGlobalPose();
		linearDamping = bodyStorage.dynamicActor->getLinearDamping();
		mass = bodyStorage.dynamicActor->getMass();
		massInertiaTensor = bodyStorage.dynamicActor->getMassSpaceInertiaTensor();
		maxAngleVel = bodyStorage.dynamicActor->getMaxAngularVelocity();
		numShapes = bodyStorage.dynamicActor->getNbShapes();
		rigidFlag = bodyStorage.dynamicActor->getRigidBodyFlags();
		actorType = bodyStorage.dynamicActor->getType();
		dynaFlag = bodyStorage.dynamicActor->getRigidDynamicFlags();

	}
	else if(bodyType == STATIC)
	{
		baseFlag = bodyStorage.staticActor->getBaseFlags();
		actorFlag = bodyStorage.staticActor->getActorFlags();
		concreteType = bodyStorage.staticActor->getConcreteType();
		globalPose = bodyStorage.staticActor->getGlobalPose();
		numShapes = bodyStorage.staticActor->getNbShapes();
		actorType = bodyStorage.staticActor->getType();
	}

	std::string baseFlags, actorFlags, typeFlag, actorTypeString, rigidBodyFlags;

	if(baseFlag.isSet(PxBaseFlag::eIS_RELEASABLE))
		baseFlags += "Releasable & ";
	if(baseFlag.isSet(PxBaseFlag::eOWNS_MEMORY))
		baseFlags += "Owns Memory ";

	if(actorFlag.isSet(physx::PxActorFlag::eDISABLE_GRAVITY))
		actorFlags += "Disabled Gravity & ";
	if(actorFlag.isSet(physx::PxActorFlag::eDISABLE_SIMULATION))
		actorFlags += "Disable Simulation & ";
	if(actorFlag.isSet(physx::PxActorFlag::eSEND_SLEEP_NOTIFIES))
		actorFlags += "Send Sleep Notifies & ";
	if(actorFlag.isSet(PxActorFlag::eVISUALIZATION))
		actorFlags += "Visualization ";

	if(concreteType == physx::PxConcreteType::eUNDEFINED)
		typeFlag = "Undefined";
	else if(concreteType == physx::PxConcreteType::eTRIANGLE_MESH)
		typeFlag = "Triangle Mesh";
	else if(concreteType == physx::PxConcreteType::eSHAPE)
		typeFlag = "Shape";
	else if(concreteType == physx::PxConcreteType::eRIGID_STATIC)
		typeFlag = "Rigid Static";
	else if(concreteType == physx::PxConcreteType::eRIGID_DYNAMIC)
		typeFlag = "Rigid Dynamic";
	else if(concreteType == physx::PxConcreteType::ePARTICLE_SYSTEM)
		typeFlag = "Particle System";
	else if(concreteType == physx::PxConcreteType::ePARTICLE_FLUID)
		typeFlag = "Particle Fluid";
	else if(concreteType == physx::PxConcreteType::eMATERIAL)
		typeFlag = "Material";
	else if(concreteType == physx::PxConcreteType::eCONVEX_MESH)
		typeFlag = "Convex Mesh";
	else
		typeFlag = "Something Else";

	if(actorType == PxActorType::eRIGID_STATIC)
		actorTypeString = "Rigid Static";
	else if(actorType == PxActorType::eRIGID_DYNAMIC)
		actorTypeString = "Rigid Dynamic";
	else if(actorType == PxActorType::ePARTICLE_FLUID)
		actorTypeString = "Particle Fluid";
	else if(actorType == PxActorType::ePARTICLE_SYSTEM)
		actorTypeString = "Particle System";

	if(rigidFlag.isSet(PxRigidBodyFlag::eKINEMATIC))
		rigidBodyFlags += "Kinematic & ";
	else if(rigidFlag.isSet(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES))
		rigidBodyFlags += "Kinematic Target Scene Queries & ";
	else if(rigidFlag.isSet(PxRigidBodyFlag::eENABLE_CCD))
		rigidBodyFlags += "CCD Enabled & ";
	
	
		
	printf("Rigid Body Component Debug: \n");
	printf("Base Flags: "); printf(baseFlags.c_str());printf("\n");
	printf("Actor Flags: "); printf(actorFlags.c_str());printf("\n");
	printf("Angle Damping: %f\n", angleDamping);
	printf("Centre Of Mass pos: %f, %f, %f    rot: %f, %f, %f, %f\n", cMassLocalPose.p.x, cMassLocalPose.p.y, cMassLocalPose.p.z,
		cMassLocalPose.q.x, cMassLocalPose.q.y, cMassLocalPose.q.z, cMassLocalPose.q.w);
	printf("Concrete Type: "); printf(typeFlag.c_str());printf("\n");
	printf("Global Pose pos: %f, %f, %f     rot: %f, %f, %f, %f\n", globalPose.p.x, globalPose.p.y, globalPose.p.z, globalPose.q.x,
		globalPose.q.y, globalPose.q.z, globalPose.q.w);
	printf("Linear Damping: %f\n", linearDamping);
	printf("Mass: %f\n", mass);
	printf("Max Angle Velocity: %f\n", maxAngleVel);
	printf("Mass Inertia Tensor: %f, %f, %f\n", massInertiaTensor.x, massInertiaTensor.y, massInertiaTensor.z);
	printf("Number Shapes: %i\n", numShapes);
	printf("Rigid Flags: "); printf(rigidBodyFlags.c_str());printf("\n");
	printf("Actor Type: "); printf(actorTypeString.c_str()); printf("\n");
	if(dynaFlag.isSet(PxRigidDynamicFlag::eKINEMATIC))
		printf("Is Kinematic\n");
}

float RigidBodyComponent::GetMass()const
{
	if(bodyType == DYNAMIC)
		return bodyStorage.dynamicActor->getMass();

	return 0.0f;
}

void RigidBodyComponent::SetMass(const float newMass)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->setMass(newMass);
}

void RigidBodyComponent::SetVelocity(physx::PxVec3& newVel)
{
	if(bodyType == DYNAMIC)
	{
		bodyStorage.dynamicActor->setLinearVelocity(newVel);
	}
}

physx::PxVec3 RigidBodyComponent::GetVelocity() const
{
	if(bodyType == DYNAMIC)
		return bodyStorage.dynamicActor->getLinearVelocity();

	return physx::PxVec3(PxZero);
}

physx::PxVec3 RigidBodyComponent::GetPosition() const
{
	if(bodyType == DYNAMIC)
		return bodyStorage.dynamicActor->getGlobalPose().p;
	else if(bodyType == STATIC)
		return bodyStorage.staticActor->getGlobalPose().p;

	return physx::PxVec3(PxZero);
}

physx::PxQuat RigidBodyComponent::GetOrientation() const
{
	if(bodyType == DYNAMIC)
		return bodyStorage.dynamicActor->getGlobalPose().q;
	else if(bodyType == STATIC)
		return bodyStorage.staticActor->getGlobalPose().q;

	return physx::PxQuat(PxIdentity);
}

void RigidBodyComponent::ClearForces()
{
	if(bodyType==DYNAMIC)
	{
		bodyStorage.dynamicActor->clearForce(physx::PxForceMode::eFORCE);
		bodyStorage.dynamicActor->clearForce(physx::PxForceMode::eIMPULSE);
		bodyStorage.dynamicActor->clearForce(physx::PxForceMode::eACCELERATION);
		bodyStorage.dynamicActor->clearForce(physx::PxForceMode::eVELOCITY_CHANGE);
	}
}

RigidBodyComponent* RigidBodyComponent::Clone(GameObject* gameObject)
{
	RigidBodyComponent* clone = new RigidBodyComponent();

	clone->enabled = enabled;
	clone->owningGameObject = gameObject;

	if(bodyType != NONE)
	{
		physx::PxRigidActor* actor = NULL;

		if(bodyType == DYNAMIC)
			actor = bodyStorage.dynamicActor;
		else if(bodyType == STATIC)
			actor = bodyStorage.staticActor;

		if(actor)
		{
			physx::PxTransform currTrans = actor->getGlobalPose();

			clone->CreateRigidBody(bodyType, currTrans.p, currTrans.q);

			PxU32 numShapes = actor->getNbShapes();
			PxShape** shapes = new physx::PxShape*[numShapes];
			actor->getShapes(shapes, numShapes);
			for (PxU32 i = 0; i < numShapes; i++)
			{
				clone->AttachShape(*shapes[i]);
			}

			delete[] shapes;

			if(physxDebugNode)
				clone->CreateDebugDraw();
		}
	}

	return clone;
}

void RigidBodyComponent::SetKinematic(const bool kinematicOn)
{
	if(bodyType == DYNAMIC)
		bodyStorage.dynamicActor->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematicOn);
}

void RigidBodyComponent::SetKinematicTarget(const physx::PxTransform& target)
{
	if(bodyType == DYNAMIC)
	{
		bodyStorage.dynamicActor->setKinematicTarget(target);
	}
}


