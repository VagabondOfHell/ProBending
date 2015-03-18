#include "ProbenderInputHandler.h"
#include "InputNotifier.h"
#include "InputManager.h"
#include "Probender.h"
#include "Arena.h"
#include "ProjectileManager.h"
#include "RigidBodyComponent.h"
#include "ParticleComponent.h"
#include "MeshRenderComponent.h"
#include "ProjectileController.h"

#include "IScene.h"
#include "OgreCamera.h"

#include "PxRigidDynamic.h"
#include "foundation/PxVec2.h"
#include "AttackDatabase.h"

const float ProbenderInputHandler::LEAN_RESET_DISTANCE = 0.15f;

const float ProbenderInputHandler::ATTACK_PAUSE = 0.1f;

ProbenderInputHandler::ProbenderInputHandler(Probender* _probenderToHandle, bool manageStance, 
					ConfigurationLayout keyLayout/* = ConfigurationLayout()*/)
					:keysLayout(keyLayout), activeAttack(NULL), NeedSpawnPosition(true)
{
	SetProbenderToHandle(_probenderToHandle);
	ManageStance = manageStance;
	canLean = true;
	attackBreather = 0.0f;
}


ProbenderInputHandler::~ProbenderInputHandler(void)
{
	//On destruction, indicate we want to stop listening to events.
	//We assume this will be destroyed before the input manager (which it should be)
	StopListeningToAll();

	//Dont delete current attack because it is only a reference to an attack in the list
}

void ProbenderInputHandler::SetProbenderToHandle(Probender* _probenderToHandle)
{
	probender = _probenderToHandle;
	//Arena* arena = probender->GetOwningArena();

	if(probender)
		GenerateGestures();
}

#pragma region GestureDatabase
void ProbenderInputHandler::GenerateGestures()
{
	mainElementGestures.clear();

	switch (probender->characterData.MainElement)
	{
	case ElementEnum::Element::Earth:
		PopulateWithGestures(mainElementGestures, ElementEnum::Earth);
		break;
	case ElementEnum::Element::Fire:
		PopulateWithGestures(mainElementGestures, ElementEnum::Fire);
		break;
	case ElementEnum::Element::Water:
		PopulateWithGestures(mainElementGestures, ElementEnum::Water);
		break;
	default:
		break;
	}
}

void ProbenderInputHandler::PopulateWithGestures(std::vector<Attack>& elementVector, ElementEnum::Element element)
{
	elementVector.clear();

	switch (element)
	{
	case ElementEnum::Earth:
		AttackDatabase::GetEarthAttacks(probender->GetOwningArena()->GetProjectileManager(), probender->contestantID, elementVector);
		break;
	case ElementEnum::Fire:
		AttackDatabase::GetFireAttacks(probender->GetOwningArena()->GetProjectileManager(), probender->contestantID, elementVector);
		break;
	case ElementEnum::Water:
		AttackDatabase::GetWaterAttacks(probender->GetOwningArena()->GetProjectileManager(), probender->contestantID, elementVector);
		break;
	case ElementEnum::Air:
		AttackDatabase::GetAirAttacks(probender->GetOwningArena()->GetProjectileManager(), probender->contestantID, elementVector);
		break;
	default:
		break;
	}
}

#pragma endregion

#pragma region Input State Controls

void ProbenderInputHandler::BeginListeningToAll()
{
	BeginListeningToKinectBody();
	BeginListeningToKinectSpeech();
	BeginListeningToOISInput();
}

void ProbenderInputHandler::StopListeningToAll()
{
	StopListeningToKinectBody();
	StopListeningToKinectSpeech();
	StopListeningToOISInput();
}

void ProbenderInputHandler::PauseListeningToAll()
{
	PauseKinectBodyListening();
	PauseKinectSpeechListening();
	PauseOISInputListening();
}

void ProbenderInputHandler::ResumeListeningToAll()
{
	ResumeKinectBodyListening();
	ResumeKinectSpeechListening();
	ResumeOISInputListening();
}

#pragma endregion

void ProbenderInputHandler::Update(const float gameTime)
{
	if(!IsListening())
	{
		InputManager* inputManager = InputManager::GetInstance();

		if(inputManager->RegisterListenerToNewBody(this))
			printf("Registered!\n");
	}

	if(attackBreather > 0.0f)
		attackBreather -= gameTime;

	//if no active attack, proceed through updating each gesture
	if(!activeAttack)
	{
		for (unsigned int i = 0; i < mainElementGestures.size(); i++)
		{
			mainElementGestures[i].Update(gameTime);
		}

		for (int i = 0; i < subElementGestures.size(); i++)
		{
			subElementGestures[i].Update(gameTime);
		}
	}
	else//otherwise only update the active
	{
		if(NeedSpawnPosition)
		{
			if(activeAttack->SpawnPositionValid())
			{
				PrepareProjectile();
				NeedSpawnPosition = false;
			}
		}

		if(activeAttack)
			activeAttack->Update(gameTime);
	}
}

#pragma region Kinect Input

void ProbenderInputHandler::BodyAcquired()
{
	printf("Body ID: %i\n", GetBody()->GetBodyTrackingID());
	printf("Body Index: %i \n", GetBody()->GetBodyID());
}

void ProbenderInputHandler::BodyLost(const CompleteData& currentData, const CompleteData& previousData)
{
	InputManager::GetInstance()->UnregisterBodyListener(this);
	printf("Body Lost\n");
}
	
void ProbenderInputHandler::HandTrackingStateChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData)
{
	//printf("Hand Track");

}

void ProbenderInputHandler::HandConfidenceChanged(const Hand hand, const CompleteData& currentData, const CompleteData& previousData)
{
	
}

void ProbenderInputHandler::BodyFrameAcquired(const CompleteData& currentData, const CompleteData& previousData)
{
	UpdateDisplay(currentData);

	CheckLean(currentData, previousData);

	CheckJump(currentData, previousData);

	AttackData frameData = AttackData();
	frameData.CurrentData = &currentData;
	frameData.PreviousData = &previousData;
	frameData._BodyDimensions = &bodyDimensions;
	frameData._Probender = probender;

	HandleAttacks(frameData);
}

void ProbenderInputHandler::UpdateDisplay(const CompleteData& currentData)
{
	std::vector<Ogre::Vector3> meshData = std::vector<Ogre::Vector3>();
	meshData.reserve(JointType::JointType_Count);

	CameraSpacePoint spineBasePoint = currentData.JointData[JointType_SpineBase].Position;
	Ogre::Vector3 spineBasePosition = Ogre::Vector3(-spineBasePoint.X, spineBasePoint.Y, 1.0f);

	for (int i = 0; i < RenderableJointType::Count; i++)
	{
		if(currentData.JointData->TrackingState != TrackingState::TrackingState_NotTracked)
		{
			CameraSpacePoint point = currentData.JointData[i].Position;
			
			meshData.push_back((Ogre::Vector3(-point.X * PROBENDER_HALF_EXTENTS.x * 2, point.Y
				* PROBENDER_HALF_EXTENTS.y * 1.5f, 1.0f)) - spineBasePosition);
		}
	}

	probender->meshRenderComponent->UpdateMesh(meshData, 0, Ogre::VES_POSITION);
}

void ProbenderInputHandler::CheckLean(const CompleteData& currentData, const CompleteData& previousData)
{
	if(!canLean)
	{
		if(currentData.LeanTrackState == TrackingState::TrackingState_Tracked)
			if(Ogre::Math::Abs(currentData.LeanAmount.X ) < LEAN_RESET_DISTANCE)
				canLean = true;
	}

	//if current and previous leans are tracked, check for the beginning of a lean by checking that if we are leaning right
	//that current data exceeds previous data or if we are leaning left that current data is less than previous data. If 
	//opposite, that means we are returning from a lean
	if(canLean && (currentData.LeanTrackState == TrackingState::TrackingState_Tracked && previousData.LeanTrackState == TrackingState::TrackingState_Tracked)
		&& ((currentData.LeanAmount.X > 0.0f && previousData.LeanAmount.X > 0.0f && currentData.LeanAmount.X > previousData.LeanAmount.X) ||
		currentData.LeanAmount.X < 0.0f && previousData.LeanAmount.X < 0.0f && currentData.LeanAmount.X < previousData.LeanAmount.X))
	{
		if(Ogre::Math::Abs(currentData.LeanAmount.X - previousData.LeanAmount.X) > controlOptions.LeanThreshold)
		{
			Probender::DodgeDirection dir = Probender::DD_RIGHT;

			if(currentData.LeanAmount.X > 0.0f)
				dir = Probender::DD_LEFT;

			probender->Dodge(dir);
			canLean = false;
		}
	}
}

void ProbenderInputHandler::CheckJump(const CompleteData& currentData, const CompleteData& previousData)
{
	//ensure all data is currently tracked (in order to try to avoid ghost jumps)
	if(currentData.JointData[JointType::JointType_FootLeft].TrackingState == TrackingState::TrackingState_Tracked &&
		currentData.JointData[JointType::JointType_FootRight].TrackingState == TrackingState::TrackingState_Tracked &&
		previousData.JointData[JointType::JointType_FootLeft].TrackingState == TrackingState::TrackingState_Tracked &&
		previousData.JointData[JointType::JointType_FootRight].TrackingState == TrackingState::TrackingState_Tracked)
	{
		if((currentData.JointData[JointType_FootLeft].Position.Y - previousData.JointData[JointType_FootLeft].Position.Y)
			>= controlOptions.JumpThreshold &&
			(currentData.JointData[JointType_FootRight].Position.Y - previousData.JointData[JointType_FootRight].Position.Y)
			>= controlOptions.JumpThreshold)
		{
			probender->Jump();
		}
	}
}

void ProbenderInputHandler::HandleAttacks(const AttackData& attackData)
{
	StateFlags::PossibleStates currState = probender->stateManager.GetCurrentState();

	if(currState == StateFlags::IDLE_STATE)
	{
		if(!activeAttack && attackBreather <= 0.0f)
		{
			for (auto start = mainElementGestures.begin(); start != mainElementGestures.end(); ++start)
			{
				Attack::AttackState result = start->Evaluate(attackData);

				if(result == Attack::AS_CREATED)
				{
					activeAttack = &(*start);		

					if(activeAttack->SpawnPositionValid())
					{
						PrepareProjectile();
						NeedSpawnPosition = false;

						if(activeAttack->LaunchOnCreate)
							PrepareProjectileLaunch();
					}

					break;
				}
			}
		}
		else if(activeAttack)
			activeAttack->Evaluate(attackData);
	}
	else if(currState == StateFlags::ATTACKING_STATE)
	{
		Attack::AttackState result = activeAttack->Evaluate(attackData);

		if(result == Attack::AS_LAUNCHED)
			PrepareProjectileLaunch();			
	}
}

void ProbenderInputHandler::PrepareProjectile()
{
	Ogre::Vector3 spawnPos;

	ProjectileManager* projectileManager = probender->GetOwningArena()->GetProjectileManager();

	SharedProjectile proj = projectileManager->
		CreateProjectile(activeAttack->GetProjectileID());

	if(proj.get() == NULL ||
		!probender->stateManager.SetState(StateFlags::ATTACKING_STATE, 0.0f))
	{
		activeAttack->Reset();
		activeAttack = NULL;

		if(proj.get() != NULL)
			proj->Disable();

		NeedSpawnPosition = true;
		return;
	}

	proj->CasterContestantID = probender->contestantID;
	
	spawnPos = activeAttack->GetSpawnPosition();
	spawnPos.z = 1.0f;

	spawnPos = probender->GetWorldOrientation() * spawnPos;

	Ogre::Vector3 camForward = probender->camera->getDerivedOrientation() * -Ogre::Vector3::UNIT_Z;
	camForward.normalise();

	proj->SetWorldPosition(probender->GetWorldPosition() + spawnPos
		+ (camForward * proj->GetHalfExtents().x ));

	proj->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

	proj->GetRigidBody()->SetUseGravity(false);

	activeAttack->SetActiveProjectile(proj.get(), true);
}

void ProbenderInputHandler::PrepareProjectileLaunch()
{
	Projectile* proj = activeAttack->GetProjectile();

	if(proj)
	{
		proj->GetRigidBody()->WakeUp();
		proj->GetRigidBody()->SetUseGravity(proj->GetUseGravity());
		Ogre::Vector3 camForward = probender->camera->getDerivedOrientation() * -Ogre::Vector3::UNIT_Z;

		//Launch the Projectile
		proj->LaunchProjectile(HelperFunctions::OgreToPhysXVec3(camForward), 
			probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
			probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());

		activeAttack->SetActiveProjectile(NULL);

		activeAttack->Reset();
		probender->stateManager.SetStateImmediate(StateFlags::IDLE_STATE, 0.0f);
		attackBreather = ATTACK_PAUSE;
		activeAttack = NULL;

		NeedSpawnPosition = true;
	}
}

bool created = false;

void ProbenderInputHandler::DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults)
{
	AttackData frameData = AttackData();
	std::remove_const<const std::vector<KinectGestureResult>>::type noConstResults = discreteGestureResults;

	frameData.DiscreteGestureResults = &noConstResults;
	frameData._BodyDimensions = &bodyDimensions;
	frameData._Probender = probender;

	HandleAttacks(frameData);
}

void ProbenderInputHandler::ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults)
{

}

#pragma endregion

#pragma region Speech Input

void ProbenderInputHandler::AudioDataReceived(AudioData* audioData)
{
	if(audioData)
	{
		std::wstring data = L"Data" + audioData->CommandValue;
		wprintf(data.c_str());
		std::wstring child = L"Child Data: " + audioData->ChildData->CommandValue;
		wprintf(child.c_str());

		std::wcout << L"Data: " << audioData->CommandValue.c_str() << std::endl;
		std::wcout << L"Child Data: " << audioData->ChildData->CommandValue.c_str() << std::endl;

		if(audioData->ChildData->CommandValue == L"AIR")
		{
			probender->SetCurrentElement(ElementEnum::Air);
		}
		else if(audioData->ChildData->CommandValue == L"EARTH")
		{
			probender->SetCurrentElement(ElementEnum::Earth);
		}
		else if(audioData->ChildData->CommandValue == L"FIRE")
		{
			probender->SetCurrentElement(ElementEnum::Fire);
		}
		else if(audioData->ChildData->CommandValue == L"WATER")
		{
			probender->SetCurrentElement(ElementEnum::Water);
		}
		//if(!reader->GetIsPaused())
		//	reader->Pause();
	}
	//*quit = true;
}

#pragma endregion

#pragma region Mouse and Keyboard Input

void ProbenderInputHandler::keyDown(const OIS::KeyEvent &arg)
{
}

void ProbenderInputHandler::keyPressed( const OIS::KeyEvent &arg )
{
	if(arg.key == OIS::KC_F)
	{
		probender->SetCurrentElement(ElementEnum::Element::Fire);
	}
	else if(arg.key == OIS::KC_E)
	{
		probender->SetCurrentElement(ElementEnum::Element::Earth);
	}
	else if(arg.key == OIS::KC_UP)
	{
		if(probender->rightHandAttack)
		{
			RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);

			if(rigidBody)
				rigidBody->ApplyForce(physx::PxVec3(0.0f, 0.0f, -5.0f));
		}
		else
		{
			Ogre::Vector3 newPos = probender->GetWorldPosition();
			newPos.x += 1;
			probender->rigidBody->SetPosition(HelperFunctions::OgreToPhysXVec3(newPos));
			//printf("New Position: %f, %f, %f\n", newPos.x, newPos.y, newPos.z);
		}
	}
	else if(arg.key == OIS::KC_DOWN)
	{
		Ogre::Vector3 newPos = probender->GetWorldPosition();
		newPos.x -= 1;
		probender->rigidBody->SetPosition(HelperFunctions::OgreToPhysXVec3(newPos));
		//printf("New Position: %f, %f, %f\n", newPos.x, newPos.y, newPos.z);
	}
	else if(arg.key == OIS::KC_LEFT)
	{
		if(probender->rightHandAttack)
		{
			RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);
			Ogre::Vector3 currPos = probender->rightHandAttack->GetWorldPosition();
			if(rigidBody)
				rigidBody->ApplyForce(physx::PxVec3(-100.0f, 0.0f, 0.0f));
		}
		else
		{
			probender->Dodge(Probender::DD_LEFT);
		}
	}
	else if(arg.key == OIS::KC_RIGHT)
	{
		if(probender->rightHandAttack)
		{
			RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);

			if(rigidBody)
				rigidBody->ApplyForce(physx::PxVec3(100.0f, 0.0f, 0.0f));
		}
		else
		{
			probender->Dodge(Probender::DD_RIGHT);
		}
	}
	else if(arg.key == keysLayout.JumpButton)
	{
		probender->Jump();
	}
	else if(arg.key == keysLayout.AttackButton)
	{
		if(probender->GetCurrentElement() == ElementEnum::Fire)
		{
			SharedProjectile attack = probender->GetOwningArena()->GetProjectileManager()->
				CreateProjectile(ElementEnum::Fire, AbilityIDs::FIRE_JAB);

			if(attack)
			{
				attack->CasterContestantID = probender->contestantID;

				attack->Enable();
				attack->GetRigidBody()->SetUseGravity(attack->GetUseGravity());

				Ogre::Vector3 camDir = probender->Forward();

				attack->SetWorldPosition((probender->GetWorldPosition() +
					probender->Forward() * 2.0f));
				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

				/*((RigidBodyComponent*)attack->GetComponent(Component::RIGID_BODY_COMPONENT))->ApplyImpulse(
					physx::PxVec3(camDir.x, camDir.y, camDir.z) * 20.0f);*/
				/*attack->LaunchProjectile(physx::PxVec3(camDir.x, camDir.y, camDir.z), 
					probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed());*/

				probender->rightHandAttack = attack;
			}
			
		}
		else if(probender->GetCurrentElement() == ElementEnum::Earth)
		{
			SharedProjectile attack = probender->GetOwningArena()->
				GetProjectileManager()->CreateProjectile(ElementEnum::Earth, AbilityIDs::EARTH_JAB);

			if(attack)
			{
				attack->CasterContestantID = probender->contestantID;

				attack->Enable();
				attack->GetRigidBody()->SetUseGravity(true);

				MeshRenderComponent* renderComp = (MeshRenderComponent*)attack->GetComponent(Component::MESH_RENDER_COMPONENT);

				Ogre::Vector3 camDir = probender->Forward();

				attack->SetWorldPosition(probender->GetWorldPosition() + 
					(probender->Forward() * (renderComp->GetHalfExtents().x * 2)));
				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

				attack->LaunchProjectile(physx::PxVec3(camDir.x, camDir.y, camDir.z), 
					probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
					probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());

				probender->rightHandAttack = attack;
			}
		}
		else if(probender->GetCurrentElement() == ElementEnum::Water)
		{
			SharedProjectile attack = probender->GetOwningArena()->
				GetProjectileManager()->CreateProjectile(ElementEnum::Water, AbilityIDs::WATER_JAB);

			if(attack)
			{
				attack->CasterContestantID = probender->contestantID;

				attack->Enable();
				attack->GetRigidBody()->SetUseGravity(true);

				MeshRenderComponent* renderComp = (MeshRenderComponent*)attack->GetComponent(Component::MESH_RENDER_COMPONENT);

				Ogre::Vector3 camDir = probender->Forward();

				attack->SetWorldPosition(probender->GetWorldPosition() + 
					(probender->Forward() * (renderComp->GetHalfExtents().x * 2)));
				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

				/*attack->LaunchProjectile(physx::PxVec3(camDir.x, camDir.y, camDir.z), 
				probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
				probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());*/

				probender->rightHandAttack = attack;
			}
		}
		
	}
	else if(arg.key == keysLayout.StopListeningButton)
	{
		StopListeningToKinectBody();
		//InputManager::GetInstance()->FlushListeners();
		std::vector<Ogre::Vector3> meshData = std::vector<Ogre::Vector3>();
		meshData.reserve(JointType::JointType_Count);

		for (int i = 0; i < RenderableJointType::Count; i++)
		{
				meshData.push_back(Ogre::Vector3(0.0f));
		}

		probender->meshRenderComponent->UpdateMesh(meshData, 0, Ogre::VES_POSITION);
	}
	else if (arg.key == OIS::KC_BACK)
	{
	}
	else if(arg.key == OIS::KC_B)
	{
		if(probender->rightHandAttack)
		{
			ParticleComponent* parts = (ParticleComponent*)probender->rightHandAttack->
				GetComponent(Component::PARTICLE_COMPONENT);

			if(parts)
			{
				parts->SetTransformationSpace(!parts->GetTransformationSpace());
			}
		}
	}
}

void ProbenderInputHandler::keyReleased( const OIS::KeyEvent &arg )
{
}

physx::PxVec2 prev = physx::PxVec2(0.0f, 0.0f);

void ProbenderInputHandler::mouseMoved( const OIS::MouseEvent &arg )
{
	using namespace physx;

	PxVec2 curr = PxVec2(arg.state.X.abs, arg.state.Y.abs);

	if(probender->rightHandAttack != nullptr)
	{
		PxVec3 dir = PxVec3(curr.x - prev.x, -(curr.y - prev.y), 0.0f).getNormalized();

		//printf("Direction: %f, %f, %f\n", dir.x, dir.y, dir.z);

		RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);

		if(rigidBody)
			rigidBody->ApplyImpulse(dir * 250.0f);
	}
	
	prev = curr;
}

void ProbenderInputHandler::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
}

void ProbenderInputHandler::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
}

bool ProbenderInputHandler::ListenToBody(UINT8 bodyIndex)
{
	if(bodyIndex >= 0)
		return InputManager::GetInstance()->RegisterListenerToBody(bodyIndex, this);
	
	return false;
}

#pragma endregion