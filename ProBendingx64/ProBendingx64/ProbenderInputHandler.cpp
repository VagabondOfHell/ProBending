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

ProbenderInputHandler::ProbenderInputHandler(Probender* _probenderToHandle, bool manageStance, 
					ConfigurationLayout keyLayout/* = ConfigurationLayout()*/)
					:keysLayout(keyLayout), activeAttack(NULL)
{
	SetProbenderToHandle(_probenderToHandle);
	ManageStance = manageStance;
	canLean = true;

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
		AttackDatabase::GetEarthAttacks(probender->GetOwningArena()->GetProjectileManager(), elementVector);
		break;
	case ElementEnum::Fire:
		AttackDatabase::GetFireAttacks(probender->GetOwningArena()->GetProjectileManager(), elementVector);
		break;
	case ElementEnum::Water:
		AttackDatabase::GetWaterAttacks(probender->GetOwningArena()->GetProjectileManager(), elementVector);
		break;
	case ElementEnum::Air:
		AttackDatabase::GetAirAttacks(probender->GetOwningArena()->GetProjectileManager(), elementVector);
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
		activeAttack->Update(gameTime);
	}
}

#pragma region Kinect Input

void ProbenderInputHandler::BodyAcquired()
{
}

void ProbenderInputHandler::BodyLost(const CompleteData& currentData, const CompleteData& previousData)
{
	InputManager::GetInstance()->UnregisterBodyListener(this);
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
	
	/*if(probender->rightHandAttack)
	{
		ProjectileController* controller = probender->rightHandAttack->GetController();

		if(controller)
			controller->ControlProjectile(probender, bodyDimensions, currentData, previousData);
	}*/
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
			meshData.push_back(Ogre::Vector3(-point.X, point.Y, 1.0f) - spineBasePosition);
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
		if(!activeAttack)
		{
			for (auto start = mainElementGestures.begin(); start != mainElementGestures.end(); ++start)
			{
				Attack::AttackState result = start->Evaluate(attackData);

				if(result == Attack::AS_CREATED)
				{
					activeAttack = &(*start);

					ProjectileManager* projectileManager = probender->GetOwningArena()->GetProjectileManager();

					SharedProjectile proj = projectileManager->
						CreateProjectile(activeAttack->GetProjectileID());

					if(proj.get() == NULL ||
						!probender->stateManager.SetState(StateFlags::ATTACKING_STATE, 0.0f))
					{
						activeAttack->Reset();
						activeAttack = NULL;
						break;
					}

					proj->SetWorldPosition(probender->GetWorldPosition() +
						probender->Forward() * 2.0f);
					proj->GetRigidBody()->SetUseGravity(false);
					//proj->GetRigidBody()->PutToSleep();

					activeAttack->SetActiveProjectile(proj.get(), true);

					break;
				}
			}
		}
	}
	else if(currState == StateFlags::ATTACKING_STATE)
	{
		Attack::AttackState result = activeAttack->Evaluate(attackData);

		if(result == Attack::AS_LAUNCHED)
		{
			Projectile* proj = activeAttack->GetProjectile();

			proj->GetRigidBody()->WakeUp();
			proj->GetRigidBody()->SetUseGravity(true);
			//Launch the Projectile
			proj->LaunchProjectile(HelperFunctions::OgreToPhysXVec3(probender->Forward()), 15.0f);
			
			activeAttack->Reset();
			activeAttack = NULL;
			probender->stateManager.SetState(StateFlags::IDLE_STATE, 0.0f);
		}
	}
}

bool created = false;

void ProbenderInputHandler::DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults)
{
	///BE SURE TO IMPLEMENT MANAGE STANCE////
	for (int i = 0; i < discreteGestureResults.size(); i++)
	{
		if(discreteGestureResults[i].discreteConfidence > 0.2f)
		{
			if(discreteGestureResults[i].gestureName == L"Earth_Grounded")
			{
				//printf("Grounded\n");
			}
			else if(discreteGestureResults[i].gestureName == L"Fire_Blast_Begin" && discreteGestureResults[i].discreteConfidence >= 0.7f)
				if(!created && probender->GetCurrentElement() == ElementEnum::Fire)
				{
					SharedProjectile attack = probender->GetOwningArena()->
						GetProjectileManager()->CreateProjectile(ElementEnum::Fire, AbilityIDs::FIRE_BLAST);

					//attack->AttachAbility(ability);

					probender->rightHandAttack = attack;
					created = true;
				}
			else if(discreteGestureResults[i].gestureName == L"Fire_Blast_Release")
			{
				if(probender->rightHandAttack && created)
				{
					RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);
					
					if(rigidBody)
						rigidBody->ApplyImpulse(physx::PxVec3(0.0f, 0.0f, -2000.0f));
					
					/*physx::PxRigidDynamic* dy = probender->rightHandAttack->GetDynamicRigidBody();
					if(dy)
					{
						dy->addForce(physx::PxVec3(0.0f, 0.0f, -2000.0f), physx::PxForceMode::eIMPULSE);
					}*/

					created = false;
				}
			}
			else if(discreteGestureResults[i].gestureName == L"Earth_Knee_Lift_Left")
				printf("Earth_Knee_Lift_Left\n");
			else if(discreteGestureResults[i].gestureName == L"Earth_Knee_Lift_Right")
				printf("Earth_Knee_Lift_Right\n");
		}
		
	}
	//Fire_Blast_Begin, Fire_Blast_Release, Earth_Knee_Lift_Left, Earth_Knee_Lift_Right, Earth_Grounded
}

void ProbenderInputHandler::ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults)
{
	///BE SURE TO IMPLEMENT MANAGE STANCE////
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

bool ProbenderInputHandler::keyDown(const OIS::KeyEvent &arg)
{
	return true;
}

bool ProbenderInputHandler::keyPressed( const OIS::KeyEvent &arg )
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
				rigidBody->ApplyForce(physx::PxVec3(0.0f, 0.0f, -1000.0f));
		}
		else
		{
			Ogre::Vector3 newPos = probender->GetWorldPosition();
			newPos.x += 1;
			probender->rigidBody->SetPosition(HelperFunctions::OgreToPhysXVec3(newPos));
			printf("New Position: %f, %f, %f\n", newPos.x, newPos.y, newPos.z);
		}
	}
	else if(arg.key == OIS::KC_DOWN)
	{
		Ogre::Vector3 newPos = probender->GetWorldPosition();
		newPos.x -= 1;
		probender->rigidBody->SetPosition(HelperFunctions::OgreToPhysXVec3(newPos));
		printf("New Position: %f, %f, %f\n", newPos.x, newPos.y, newPos.z);
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
			/*Ogre::Vector3 newPos = probender->GetWorldPosition();
			newPos.z -= 1;
			probender->rigidBody->SetPosition(HelperFunctions::OgreToPhysXVec3(newPos));
			printf("New Position: %f, %f, %f\n", newPos.x, newPos.y, newPos.z);*/
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
			/*Ogre::Vector3 newPos = probender->GetWorldPosition();
			newPos.z += 1;
			probender->rigidBody->SetPosition(HelperFunctions::OgreToPhysXVec3(newPos));
			printf("New Position: %f, %f, %f\n", newPos.x, newPos.y, newPos.z);*/
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
			SharedProjectile attack = probender->GetOwningArena()->
				GetProjectileManager()->CreateProjectile(ElementEnum::Fire, AbilityIDs::FIRE_BLAST);

			//attack->AttachAbility(ability);

			probender->rightHandAttack = attack;
			
		}
		else if(probender->GetCurrentElement() == ElementEnum::Earth)
		{
			SharedProjectile attack = probender->GetOwningArena()->
				GetProjectileManager()->CreateProjectile(ElementEnum::Earth, AbilityIDs::EARTH_COIN);

			if(attack)
			{
				attack->Enable();
				attack->GetRigidBody()->SetUseGravity(true);
				//attack->AttachAbility(ability);
				//Ogre::Vector3 camDir = probender->owningArena->GetOwningScene()->GetCamera()->getDirection();
				Ogre::Vector3 camDir = probender->Forward();

				attack->SetWorldPosition((probender->GetWorldPosition() +
					probender->Forward() * 2.0f));
				/*attack->SetWorldPosition(probender->owningArena->GetOwningScene()->GetCamera()->getPosition()
					+ (camDir * 2.0f));*/

				/*attack->GetController()->ProjectileOrigin = HelperFunctions::OgreToPhysXVec3(attack->GetWorldPosition());
				attack->GetController()->ProbenderForward = HelperFunctions::OgreToPhysXVec3(probender->Forward());
				attack->GetController()->ProbenderRight = HelperFunctions::OgreToPhysXVec3(probender->Right());
				attack->GetController()->ProbenderUp = HelperFunctions::OgreToPhysXVec3(probender->Up());*/

				((RigidBodyComponent*)attack->GetComponent(Component::RIGID_BODY_COMPONENT))->ApplyImpulse(
					physx::PxVec3(camDir.x, camDir.y, camDir.z) * 20.0f);/**/

				probender->rightHandAttack = attack;
			}
		}
		
	}
	else if(arg.key == OIS::KC_G)
	{
		if(probender->rightHandAttack)
		{
std::vector<Ogre::Vector3> bodyPoints = std::vector<Ogre::Vector3>();

		bodyPoints.push_back(Ogre::Vector3(10.0f, 0.0f, 0.0f));
		bodyPoints.push_back(Ogre::Vector3(0.0f, 10.0f, 0.0f));
		((MeshRenderComponent*)probender->rightHandAttack->GetComponent(Component::MESH_RENDER_COMPONENT))->
			UpdateMesh(bodyPoints, 0, Ogre::VES_POSITION);
		}
		

		//((MeshRenderComponent*)probender->GetComponent(Component::MESH_RENDER_COMPONENT))->UpdateMesh(bodyPoints, 0, Ogre::VES_POSITION);
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

	return true;
}

bool ProbenderInputHandler::keyReleased( const OIS::KeyEvent &arg )
{
	return true;
}

physx::PxVec2 prev = physx::PxVec2(0.0f, 0.0f);

bool ProbenderInputHandler::mouseMoved( const OIS::MouseEvent &arg )
{
	using namespace physx;

	PxVec2 curr = PxVec2(arg.state.X.abs, arg.state.Y.abs);

	if(probender->rightHandAttack != nullptr)
	{
		PxVec3 dir = PxVec3(curr.x - prev.x, -(curr.y - prev.y), 0.0f).getNormalized();

		printf("Direction: %f, %f, %f\n", dir.x, dir.y, dir.z);

		RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);

		if(rigidBody)
			rigidBody->ApplyImpulse(dir * 250.0f);
	}
	
	prev = curr;

	return true;
}

bool ProbenderInputHandler::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	
	return true;
}

bool ProbenderInputHandler::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

#pragma endregion