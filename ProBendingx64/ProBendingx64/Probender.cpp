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

const float Probender::DODGE_DISTANCE = 1.0f;

const float Probender::FALL_FORCE = -1000.0f;

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
	camera = newCamera;

	camera->setPosition(Ogre::Vector3(0.0f, PROBENDER_HALF_EXTENTS.y * 0.75f, -5.0f));

	Ogre::Vector3 currPos = GetWorldPosition();
	Ogre::Vector3 diff = currentTarget->GetWorldPosition() - currPos;
	diff.normalise();

	Ogre::Vector3 newCamPos = Ogre::Vector3(currPos.x + diff.x * -7.50, 
		PROBENDER_HALF_EXTENTS.y *2.0f, currPos.z + diff.z * -7.50f);

	camera->setPosition(newCamPos);
	camera->lookAt(currentTarget->GetWorldPosition());

	if(currentTarget)
		camera->lookAt(currentTarget->GetWorldPosition());
	else
		camera->lookAt(Ogre::Vector3(0.0f, PROBENDER_HALF_EXTENTS.y, 0.0f));
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
	shapeDef.SetBoxGeometry(physx::PxVec3(PROBENDER_HALF_EXTENTS.x * 0.5f, PROBENDER_HALF_EXTENTS.y, PROBENDER_HALF_EXTENTS.z));
	shapeDef.AddMaterial("101000");
	shapeDef.SetFilterFlags(ArenaData::CONTESTANT);
	PhysXDataManager::GetSingletonPtr()->CreateShape(shapeDef, "ProbenderShape");
	rigid->AttachShape("ProbenderShape");
	rigid->SetMass(150.0f);
	rigid->FreezeAllRotation();

	rigid->CreateDebugDraw();
	
	GameObject::Start();

	stateManager = ProbenderStateManager(this);

	progressTracker.Initialize(this);
	energyMeter.Initialize(owningScene->GetGUIManager(), contestantID);
	energyMeter.SetValue(characterData.CurrentAttributes.Energy, characterData.CurrentAttributes.GetMaxEnergy());

}

void Probender::Update(float gameTime)
{
	GameObject::Update(gameTime);

	if(currentTarget)
	{
		if(currentTarget->stateManager.GetCurrentState() != StateFlags::DODGE_STATE)
		{
			if(camera)
			{
				Ogre::Vector3 targetPos = currentTarget->GetWorldPosition();
				Ogre::Vector3 currPos = GetWorldPosition();
				Ogre::Vector3 diff = targetPos - currPos;
				diff.normalise();

				Ogre::Vector3 newCamPos = Ogre::Vector3(currPos.x + diff.x * -7.50, 
					PROBENDER_HALF_EXTENTS.y *2.0f, currPos.z + diff.z * -7.50f);

				camera->setPosition(newCamPos);

				camera->lookAt(targetPos.x, PROBENDER_HALF_EXTENTS.y * 1.75f, targetPos.z);
			}
		}
	}

	inputHandler.Update(gameTime);

	stateManager.Update(gameTime);	
	progressTracker.Update(gameTime);

	energyMeter.SetValue(characterData.CurrentAttributes.Energy, characterData.CurrentAttributes.GetMaxEnergy());

	if(!stateManager.GetOnGround())
		rigidBody->ApplyForce(physx::PxVec3(0.0f, characterData.CurrentAttributes.GetJumpHeight() * FALL_FORCE, 0.0f));

	switch (stateManager.GetCurrentState())
	{
	case StateFlags::IDLE_STATE:
		characterData.CurrentAttributes.AddEnergy(characterData.CurrentAttributes.GetEnergyRegen() * gameTime);
		break;
	case StateFlags::JUMP_STATE:
		if(rigidBody->GetVelocity().y < -0.0f && !stateManager.GetOnGround())
			stateManager.SetState(StateFlags::FALLING_STATE, 0.0f);
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
		{
			dodgeInfo.Percentile += characterData.CurrentAttributes.GetDodgeSpeed() * gameTime;

			if(dodgeInfo.Percentile >= 1.0f)
			{
				stateManager.SetState(StateFlags::IDLE_STATE, 0.0f);
				rigidBody->SetPosition(dodgeInfo.EndPos);
			}
			else
			{
				rigidBody->SetKinematicTarget(
					HelperFunctions::Lerp(dodgeInfo.StartPos, dodgeInfo.EndPos, dodgeInfo.Percentile));
			}
			
			break;
		}
	case StateFlags::REELING_STATE:
		if(rigidBody->GetVelocity().magnitudeSquared() < 0.1f)
		{
			stateManager.SetStateImmediate(StateFlags::IDLE_STATE, 0.0f);
			rigidBody->SetVelocity(physx::PxVec3(0.0f));
		}
		break;

	case StateFlags::TRANSITION_STATE:
		rigidBody->SetKinematicTarget(
			HelperFunctions::Lerp(transitionInfo.StartPos, transitionInfo.EndPos, transitionInfo.Percentile));
		transitionInfo.Percentile += 1.5f * gameTime;

		if(transitionInfo.Percentile >= 1.0f)
			stateManager.SetStateImmediate(StateFlags::IDLE_STATE, 0.0f);

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

void Probender::TransitionToPoint(physx::PxVec3& positionToMoveTo)
{
	stateManager.SetStateImmediate(StateFlags::TRANSITION_STATE, 0.0f);

	positionToMoveTo.y = PROBENDER_HALF_EXTENTS.y;

	transitionInfo.StartPos = rigidBody->GetPosition();
	transitionInfo.EndPos = positionToMoveTo;
	transitionInfo.Percentile = 0.0f;
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
		rigidBody->SetKinematic(false);
		rigidBody->SetPosition(dodgeInfo.EndPos);
		break;
	case StateFlags::REELING_STATE:
		break;
	case StateFlags::TRANSITION_STATE:
		rigidBody->SetKinematic(false);
		SetInputState(Probender::Listen);
		characterData.CurrentAttributes.Energy = characterData.CurrentAttributes.GetMaxEnergy();
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
		rigidBody->SetKinematic(true);
		break;
	case StateFlags::REELING_STATE:
		break;
	case StateFlags::TRANSITION_STATE:
		rigidBody->SetKinematic(true);
		SetInputState(Probender::Pause);
		break;
	case StateFlags::COUNT:
		break;
	default:
		break;
	}
}

void Probender::OnTriggerEnter(GameObject* trigger, GameObject* other)
{
	if(trigger->tag == TagsAndLayersManager::ArenaZoneTag)
	{
		ArenaData::Zones newZone = ArenaData::GetZoneFromString(trigger->GetName());

		ArenaData::Zones currZone = GetCurrentZone();

		if(newZone != currZone)
		{
			characterData.TeamDatas.CurrentZone = newZone;

			if(characterData.TeamDatas.Team == ArenaData::RED_TEAM && newZone < currZone ||
				characterData.TeamDatas.Team == ArenaData::BLUE_TEAM &&	newZone > currZone)
				owningArena->BeginTransition(contestantID, newZone, currZone);
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
	characterData.CurrentAttributes.AddEnergy(-damage);

	if(!stateManager.SetState(StateFlags::REELING_STATE, characterData.CurrentAttributes.GetRecoveryRate()))
	{
		stateManager.ResetCurrentState();
	}

	float energyDiff = characterData.CurrentAttributes.Energy / characterData.CurrentAttributes.GetMaxEnergy();

	rigidBody->ApplyImpulse(-HelperFunctions::OgreToPhysXVec3(Forward()) * (knockback * (1.0f - (energyDiff * 0.75f))));	
}
