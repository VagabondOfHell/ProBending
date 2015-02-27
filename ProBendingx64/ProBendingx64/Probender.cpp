#include "Probender.h"
#include "IScene.h"
#include "InputNotifier.h"
#include "NotImplementedException.h"
#include "PhysXDataManager.h"
#include "Arena.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"
#include "TagsAndLayersManager.h"

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

const physx::PxVec3 Probender::HALF_EXTENTS = physx::PxVec3(0.250f, 0.60f, 0.040f);

const float Probender::DODGE_DISTANCE = 1.0f;

const float Probender::FALL_FORCE = -350.0f;

Probender::Probender()
	: GameObject(NULL), owningArena(NULL), leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL), 
		CurrentZone(ArenaData::INVALID_ZONE), currentTeam(ArenaData::INVALID_TEAM)
{
}

Probender::Probender(const unsigned short _contestantID, Arena* _owningArena)
	: GameObject(_owningArena->GetOwningScene(), "Probender" + std::to_string(_contestantID)), 
		contestantID(_contestantID), owningArena(_owningArena), 
		leftHandAttack(NULL), rightHandAttack(NULL), currentTarget(NULL), playerColour(TeamData::INVALID_COLOUR), 
		CurrentZone(ArenaData::INVALID_ZONE), currentTeam(ArenaData::Team::INVALID_TEAM)
{
	tag = TagsAndLayersManager::ContestantTag;
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

	inputHandler.Update(gameTime);
	stateManager.Update(gameTime);	
	progressTracker.Update(gameTime);

	/*std::string message = "Current Zone for " + std::to_string(contestantID) + 
		" : " + ArenaData::GetStringFromZone(CurrentZone) + "\n";
	printf(message.c_str());*/

	StateFlags::PossibleStates ps = stateManager.GetCurrentState();

	if(!stateManager.GetOnGround())
	{
		rigidBody->ApplyForce(physx::PxVec3(0.0f, characterData.SkillsBonus.JumpHeight * FALL_FORCE, 0.0f));
	}

	switch (stateManager.GetCurrentState())
	{
	case StateFlags::IDLE_STATE:
		break;
	case StateFlags::JUMP_STATE:
		HandleJump();
		break;
	case StateFlags::FALLING_STATE:
		HandleFall();
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

void Probender::CreateInGameData(const ProbenderData& data)
{
	currentTeam = data.TeamDatas.StartTeam;
	CurrentZone = data.TeamDatas.StartZone;
	playerColour = data.TeamDatas.PlayerColour;

	characterData.FillFromProbenderData(data);

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
	
}

std::string Probender::GetMeshAndMaterialName()
{
	if(playerColour != TeamData::INVALID_COLOUR)
	{
		std::string colourString = TeamData::EnumToString(playerColour);

		return colourString + "Probender";
	}

	return "";
}

void Probender::OnCollisionEnter(const CollisionReport& collision)
{
	/*std::string message = "Collision Entered with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());
	*/
	if(collision.Collider->tag == TagsAndLayersManager::GroundTag)
	{
		stateManager.SetOnGround(true);
	}
	else if(collision.Collider->tag == TagsAndLayersManager::ProjectileTag)
	{
		//Change to use knockback resistance instead of 1.0f
		stateManager.SetState(StateFlags::REELING_STATE, 1.0f);
	}
}

void Probender::OnCollisionLeave(const CollisionReport& collision)
{
	/*std::string message = "Collision Leave with: " + collision.Collider->GetName() + "\n";

	printf(message.c_str());*/

	if(collision.Collider->tag == TagsAndLayersManager::GroundTag)
	{
		stateManager.SetOnGround(false);
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
		rigidBody->SetVelocity(physx::PxVec3(0.0f, characterData.SkillsBonus.JumpHeight, 0.0f));
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
		rigidBody->SetVelocity(dodgeDirection * characterData.SkillsBonus.DodgeSpeed);
	else
		stateManager.SetState(StateFlags::IDLE_STATE, 0.0f);
}

void Probender::HandleJump()
{
	if(rigidBody->GetVelocity().y < -0.0f && !stateManager.GetOnGround())
		stateManager.SetState(StateFlags::FALLING_STATE, 0.0f);
}

void Probender::HandleFall()
{
	//rigidBody->SetKinematicTarget(HelperFunctions::OgreToPhysXVec3(GetWorldPosition()) + physx::PxVec3(0, -0.05f, 0.0f));
}

void Probender::OnTriggerEnter(GameObject* trigger, GameObject* other)
{
	if(trigger->tag == TagsAndLayersManager::ArenaZoneTag)
	{
		ArenaData::Zones newZone = ArenaData::GetZoneFromString(trigger->GetName());

		if(newZone != CurrentZone)
		{
			CurrentZone = newZone;
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

		if((currZone == ArenaData::BLUE_ZONE_3 || currZone == ArenaData::RED_ZONE_3) && currZone == CurrentZone)
		{
			CurrentZone = ArenaData::INVALID_ZONE;
			/*std::string message = "Trigger Left with: " + trigger->GetName() + " For: " + std::to_string(contestantID) + 
				" : " + "\n";
			printf(message.c_str());*/
		}
	}
}
