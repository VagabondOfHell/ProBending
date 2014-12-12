#include "ProbenderInputHandler.h"
#include "InputNotifier.h"
#include "InputManager.h"
#include "Probender.h"
#include "Arena.h"
#include "AbilityManager.h"
#include "ProjectileManager.h"
#include "PxRigidDynamic.h"
#include "foundation/PxVec2.h"

ProbenderInputHandler::ProbenderInputHandler(Probender* _probenderToHandle, bool manageStance)
{
	probender = _probenderToHandle;
	ManageStance = manageStance;
}


ProbenderInputHandler::~ProbenderInputHandler(void)
{
	//On destruction, indicate we want to stop listening to events.
	//We assume this will be destroyed before the input manager (which it should be)
	StopListeningToAll();
}

void ProbenderInputHandler::SetProbenderToHandle(Probender* _probenderToHandle)
{
	probender = _probenderToHandle;
}

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

void ProbenderInputHandler::Update(const float gameTime)
{
	if(!IsListening())
	{
		InputManager* inputManager = InputManager::GetInstance();

		if(inputManager->RegisterListenerToNewBody(this))
			printf("Registered!");
	}
}

#pragma region Kinect Input

void ProbenderInputHandler::LeanChanged(const CompleteData& currentData, const CompleteData& previousData)
{

}

void ProbenderInputHandler::LeanTrackingStateChanged(const CompleteData& currentData, const CompleteData& previousData)
{

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
			else if(discreteGestureResults[i].gestureName == L"Fire_Blast_Begin")
				if(!created && probender->GetCurrentElement() == ElementEnum::Fire)
				{
					AbilityManager::SharedAbilityDescriptor ability =
						probender->GetOwningArena()->GetAbilityManager()->CreateAbility(ElementEnum::Fire, AbilityIDs::FIRE_JAB, probender);

					if(ability->abilityType == AbilityDescriptor::Offensive)
					{
						Projectile* attack = probender->GetOwningArena()->
							GetProjectileManager()->CreateProjectile(ElementEnum::Fire, AbilityIDs::FIRE_JAB);

						attack->AttachAbility(ability);

						probender->rightHandAttack = attack;
						created = true;
					}
				}
			else if(discreteGestureResults[i].gestureName == L"Fire_Blast_Release")
			{
				if(probender->rightHandAttack && created)
				{
					physx::PxRigidDynamic* dy = probender->rightHandAttack->GetDynamicRigidBody();
					if(dy)
					{
						dy->addForce(physx::PxVec3(0.0f, 0.0f, -2000.0f), physx::PxForceMode::eIMPULSE);
					}

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
			physx::PxRigidDynamic* dy = probender->rightHandAttack->GetDynamicRigidBody();
			if(dy)
			{
				dy->addForce(physx::PxVec3(0.0f, 0.0f, -2000.0f), physx::PxForceMode::eIMPULSE);
			}
		}
	}
	else if(arg.key == OIS::KC_SPACE)
	{
		if(probender->GetCurrentElement() == ElementEnum::Fire)
		{
			AbilityManager::SharedAbilityDescriptor ability =
				probender->GetOwningArena()->GetAbilityManager()->CreateAbility(ElementEnum::Fire, AbilityIDs::FIRE_JAB, probender);

			if(ability->abilityType == AbilityDescriptor::Offensive)
			{
				Projectile* attack = probender->GetOwningArena()->
					GetProjectileManager()->CreateProjectile(ElementEnum::Fire, AbilityIDs::FIRE_JAB);

				attack->AttachAbility(ability);

				probender->rightHandAttack = attack;
			}
		}
		else if(probender->GetCurrentElement() == ElementEnum::Earth)
		{
			AbilityManager::SharedAbilityDescriptor ability =
				probender->GetOwningArena()->GetAbilityManager()->CreateAbility(ElementEnum::Earth, AbilityIDs::EARTH_BOULDER, probender);

			if(ability->abilityType == AbilityDescriptor::Offensive)
			{
				Projectile* attack = probender->GetOwningArena()->
					GetProjectileManager()->CreateProjectile(ElementEnum::Earth, AbilityIDs::EARTH_BOULDER);

				attack->AttachAbility(ability);

				probender->rightHandAttack = attack;
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
		physx::PxRigidDynamic* dyn = probender->rightHandAttack->GetDynamicRigidBody();
		if(dyn)
		{
			dyn->addForce(dir * 250.0f, physx::PxForceMode::eIMPULSE);
			physx::PxVec3 g = dyn->getGlobalPose().p;
			printf("Pos: %f, %f, %f\n", g.x, g.y, g.z);
		}
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