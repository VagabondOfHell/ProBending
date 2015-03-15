#include "Probender.h"
#include "IScene.h"
#include "InputNotifier.h"
#include "NotImplementedException.h"
#include "PhysXDataManager.h"
#include "Arena.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"
#include "TagsAndLayersManager.h"
#include "ProjectileManager.h"

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

const physx::PxVec3 Probender::HALF_EXTENTS = physx::PxVec3(0.250f, 0.60f, 0.40f);

const float Probender::DODGE_DISTANCE = 1.0f;

const float Probender::FALL_FORCE = -350.0f;

Probender::Probender()
	: GameObject(NULL), owningArena(NULL), leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL), camera(NULL)
{
}

Probender::Probender(const unsigned short _contestantID, const ProbenderData charData, Arena* _owningArena)
	: GameObject(_owningArena->GetOwningScene(), "Probender" + std::to_string(_contestantID)), 
		contestantID(_contestantID), owningArena(_owningArena), characterData(charData), 
		leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL), camera(NULL)
{
	characterData.CurrentAttributes = characterData.BaseAttributes;
	characterData.CurrentElement = characterData.MainElement;

	tag = TagsAndLayersManager::ContestantTag;
}

Probender::~Probender(void)
{
}

void Probender::SetCamera(Ogre::Camera* newCamera)
{
	gameObjectNode->attachObject(newCamera);

	if(camera)
		gameObjectNode->detachObject(camera);

	camera = newCamera;

	camera->setPosition(Ogre::Vector3(0.0f, 1.0f, 0.0f));
	
	if(currentTarget)
		camera->lookAt(currentTarget->GetWorldPosition());
	else
		camera->lookAt(Ogre::Vector3(0.0f, 2.0f, 0.0f));
}

void Probender::Start()
{
	inputHandler.SetProbenderToHandle(this);
	if(!inputHandler.ListenToBody(characterData.BodyID))
		printf("Listen to specified body failed\n");

	SetInputState(Probender::Listen);

	characterData.BaseAttributes.Energy = characterData.CurrentAttributes.Energy = 
		characterData.BaseAttributes.GetMaxEnergy();

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
	shapeDef.SetBoxGeometry(HALF_EXTENTS);
	shapeDef.AddMaterial("101000");
	shapeDef.SetFilterFlags(ArenaData::CONTESTANT);
	PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, "ProbenderShape");
	rigid->AttachShape("ProbenderShape");
	rigid->SetMass(150.0f);
	rigid->FreezeAllRotation();
	//rigid->SetUseGravity(false);

	rigid->CreateDebugDraw();
	
	GameObject::Start();

	stateManager = ProbenderStateManager(this);

	progressTracker.Initialize(this);
}

void Probender::Update(float gameTime)
{
	GameObject::Update(gameTime);

	if(currentTarget)
		if(camera)
			camera->lookAt(currentTarget->GetWorldPosition() + Ogre::Vector3(0.0f, 1.0f, 0.0f));

	inputHandler.Update(gameTime);
	stateManager.Update(gameTime);	
	progressTracker.Update(gameTime);

	if(stateManager.GetCurrentState() == StateFlags::IDLE_STATE)
		characterData.CurrentAttributes.AddEnergy(characterData.CurrentAttributes.GetEnergyRegen() * gameTime);

	/*std::string message = "Current Zone for " + std::to_string(contestantID) + 
		" : " + ArenaData::GetStringFromZone(CurrentZone) + "\n";
	printf(message.c_str());*/

	StateFlags::PossibleStates ps = stateManager.GetCurrentState();

	if(!stateManager.GetOnGround())
	{
		rigidBody->ApplyForce(physx::PxVec3(0.0f, characterData.CurrentAttributes.GetJumpHeight() * FALL_FORCE, 0.0f));
	}

	switch (stateManager.GetCurrentState())
	{
	case StateFlags::IDLE_STATE:
		break;
	case StateFlags::JUMP_STATE:
		HandleJump();
		break;
	case StateFlags::FALLING_STATE:
		break;
	case StateFlags::BLOCK_STATE:
		break;
	case StateFlags::CATCH_STATE:
		break;
	case StateFlags::HEAL_STATE:
		break;
	case StateFlags::DODGE_STATE:
		HandleDodge(gameTime);
		break;
	case StateFlags::REELING_STATE:
		break;
	case StateFlags::COUNT:
		break;
	default:
		break;
	}
}

void Probender::AcquireNewTarget(bool toRight)
{
	throw NotImplementedException();
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
	
}

std::string Probender::GetMeshAndMaterialName()
{
	if(characterData.TeamDatas.PlayerColour != TeamData::INVALID_COLOUR)
	{
		std::string colourString = TeamData::EnumToString(characterData.TeamDatas.PlayerColour);

		return colourString + "Probender";
	}

	return "";
}

void Probender::OnCollisionEnter(const CollisionReport& collision)
{
	std::string message = "Collision Entered with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());
	
	if(collision.Collider->tag == TagsAndLayersManager::GroundTag)
	{
		stateManager.SetOnGround(true);
	}
}

void Probender::OnCollisionLeave(const CollisionReport& collision)
{
	std::string message = "Collision Leave with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());
	//if(collision.Collider)
	if(collision.Collider->tag == TagsAndLayersManager::GroundTag)
	{
		stateManager.SetOnGround(false);
	}
	else if(collision.Collider->tag == TagsAndLayersManager::ProjectileTag)
	{
		
	}
}

void Probender::StateExitted(StateFlags::PossibleStates exittedState)
{
	switch (exittedState)
	{
	case StateFlags::IDLE_STATE:
		break;
	case StateFlags::JUMP_STATE:
		break;
	case StateFlags::FALLING_STATE:
		break;
	case StateFlags::BLOCK_STATE:
		break;
	case StateFlags::CATCH_STATE:
		break;
	case StateFlags::HEAL_STATE:
		break;
	case StateFlags::DODGE_STATE:
		rigidBody->SetVelocity(physx::PxVec3(0.0f));
		SetWorldPosition(dodgeTargetPos.x, dodgeTargetPos.y, dodgeTargetPos.z);
		dodgeTargetPos = physx::PxVec3(0.0f);
		dodgeDirection = dodgeTargetPos;
		break;
	case StateFlags::REELING_STATE:
		break;
	case StateFlags::COUNT:
		break;
	default:
		break;
	}
}

void Probender::StateEntered(StateFlags::PossibleStates enteredState)
{
	switch (enteredState)
	{
	case StateFlags::IDLE_STATE:
		rigidBody->SetVelocity(physx::PxVec3(0.0f));
		break;
	case StateFlags::JUMP_STATE:
		rigidBody->SetVelocity(physx::PxVec3(0.0f, characterData.CurrentAttributes.GetJumpHeight(), 0.0f));
		break;
	case StateFlags::FALLING_STATE:	
		break;
	case StateFlags::BLOCK_STATE:
		break;
	case StateFlags::CATCH_STATE:
		break;
	case StateFlags::HEAL_STATE:
		break;
	case StateFlags::DODGE_STATE:
		break;
	case StateFlags::REELING_STATE:
		break;
	case StateFlags::COUNT:
		break;
	default:
		break;
	}
}

void Probender::HandleDodge(const float gameTime)
{
	float distanceAway = (HelperFunctions::OgreToPhysXVec3(GetWorldPosition()) - dodgeTargetPos).magnitude();

	if(distanceAway > 0.1f)
		rigidBody->SetVelocity(dodgeDirection * characterData.CurrentAttributes.GetDodgeSpeed());
	else
		stateManager.SetState(StateFlags::IDLE_STATE, 0.0f);
}

void Probender::HandleJump()
{
	if(rigidBody->GetVelocity().y < -0.0f && !stateManager.GetOnGround())
		stateManager.SetState(StateFlags::FALLING_STATE, 0.0f);
}

void Probender::OnTriggerEnter(GameObject* trigger, GameObject* other)
{
	if(trigger->tag == TagsAndLayersManager::ArenaZoneTag)
	{
		ArenaData::Zones newZone = ArenaData::GetZoneFromString(trigger->GetName());

		if(newZone != GetCurrentZone())
		{
			characterData.TeamDatas.CurrentZone = newZone;
			/*std::string message = "Trigger Entered with: " + trigger->GetName() + "For " + std::to_string(contestantID) + 
			" : "+ "\n";
			printf(message.c_str());*/
		}
	}
}

void Probender::OnTriggerLeave(GameObject* trigger, GameObject* other)
{
	if(trigger->tag == TagsAndLayersManager::ArenaZoneTag)
	{
		ArenaData::Zones currZone = ArenaData::GetZoneFromString(trigger->GetName());

		if((currZone == ArenaData::BLUE_ZONE_3 || currZone == ArenaData::RED_ZONE_3) && currZone == GetCurrentZone())
		{
			characterData.TeamDatas.CurrentZone = ArenaData::INVALID_ZONE;
		}
	}
}

void Probender::OnCollisionStay(const CollisionReport& collision)
{
}

void Probender::ApplyProjectileCollision(float damage, float knockback)
{
	characterData.CurrentAttributes.Energy -= damage;

	if(!stateManager.SetState(StateFlags::REELING_STATE, characterData.CurrentAttributes.GetRecoveryRate()))
	{
		stateManager.ResetCurrentState();
	}

	rigidBody->ApplyImpulse(-HelperFunctions::OgreToPhysXVec3(Forward()) * knockback);

	
}
