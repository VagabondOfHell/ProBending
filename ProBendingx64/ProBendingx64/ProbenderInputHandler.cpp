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
#include "OgreSkeletonInstance.h"
#include "OgreEntity.h"

#include "OgreCamera.h"
#include "OgreBone.h"
#include "PxRigidDynamic.h"
#include "foundation/PxVec2.h"
#include "AttackDatabase.h"
#include "OgreSceneNode.h"

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
		else
		{
			if(!activeAttack->GetProjectile())
			{
				activeAttack->Reset();
				activeAttack = NULL;
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

	//CheckLean(currentData, previousData);

	//CheckJump(currentData, previousData);

	AttackData frameData = AttackData();
	frameData.CurrentData = &currentData;
	frameData.PreviousData = &previousData;
	frameData._BodyDimensions = &bodyDimensions;
	frameData._Probender = probender;

	//HandleAttacks(frameData);
}

void ProbenderInputHandler::UpdateDisplay(const CompleteData& currentData)
{
	FillJointWorldOrientations(currentData);

	std::vector<Ogre::Vector3> meshData = std::vector<Ogre::Vector3>();
	meshData.reserve(JointType::JointType_Count);

	CameraSpacePoint spineBasePoint = currentData.JointData[JointType_SpineBase].Position;
	Ogre::Vector3 spineBasePosition = Ogre::Vector3(-spineBasePoint.X, spineBasePoint.Y, -spineBasePoint.Z);
	JointOrientation spineOrientation = currentData.JointOrientations[JointType_SpineBase];
	Ogre::Quaternion spineBaseOrientation = Ogre::Quaternion(spineOrientation.Orientation.w, 
		spineOrientation.Orientation.x, spineOrientation.Orientation.y, spineOrientation.Orientation.z);

	for (int i = 0; i < RenderableJointType::Count; i++)
	{
		if(currentData.JointData->TrackingState != TrackingState::TrackingState_NotTracked)
		{
			CameraSpacePoint point = currentData.JointData[i].Position;
			
			meshData.push_back((Ogre::Vector3(-point.X * PROBENDER_HALF_EXTENTS.x * 2, point.Y
				* PROBENDER_HALF_EXTENTS.y * 1.5f, 1.0f)) - spineBasePosition);
			std::string boneName = RenderableJointType::GetBoneName((RenderableJointType::RenderableJointType)i);

			switch ((RenderableJointType::RenderableJointType)i)
			{
			case RenderableJointType::Head:
				boneName =  "head";
				break;
			case RenderableJointType::Neck:
				boneName =  "head";
				//boneName = "";
				break;
			case RenderableJointType::SpineShoulder:
				boneName =  "ribs";
				//boneName = "";
				break;
			case RenderableJointType::SpineMid:
				//boneName =  "spine";
				boneName = "ribs";
				//boneName = "";
				break;
			case RenderableJointType::SpineBase:
				//boneName =  "hips";
				boneName = "";
				break;
			case RenderableJointType::ShoulderRight:
				boneName =  "shoulder.R";
				//boneName = "upper_arm.R";
				break;
			case RenderableJointType::ElbowRight:
				boneName =  "forearm.R";
				//boneName = "shoulder.R";

				//boneName = "upper_arm.R";
				break;
			case RenderableJointType::WristRight:
				//boneName =  "hand.R";
				boneName = "forearm.R";
				break;
			case RenderableJointType::HandRight:
				boneName =  "";
				break;
			case RenderableJointType::HandTipRight:
				boneName =  "";
				break;
			case RenderableJointType::ShoulderLeft:
				boneName =  "shoulder.L";
				break;
			case RenderableJointType::ElbowLeft:
				//boneName =  "forearm.L";
				boneName = "shoulder.L";
				break;
			case RenderableJointType::WristLeft:
				boneName =  "hand.L";
				break;
			case RenderableJointType::HandLeft:
				boneName =  "";
				break;
			case RenderableJointType::ThumbRight:
				boneName =  "thumb.02.R";
				break;
			case RenderableJointType::HandTipLeft:
				boneName =  "";
				break;
			case RenderableJointType::ThumbLeft:
				boneName =  "thumb.02.L";
				break;
			case RenderableJointType::HipLeft:
				//boneName =  "thigh.L";
				boneName =  "";
				break;
			case RenderableJointType::KneeLeft:
				boneName =  "shin.L";
				break;
			case RenderableJointType::AnkleLeft:
				boneName =  "heel.L";
				break;
			case RenderableJointType::FootLeft:
				boneName =  "foot.L";
				break;
			case RenderableJointType::HipRight:
				//boneName =  "thigh.R";
				boneName =  "";
				break;
			case RenderableJointType::KneeRight:
				boneName =  "shin.R";
				break;
			case RenderableJointType::AnkleRight:
				boneName =  "heel.R";
				break;
			case RenderableJointType::FootRight:
				boneName =  "foot.R";
				break;
			case RenderableJointType::Count:
				boneName =  "";
				break;
			default:
				boneName =  "";
				break;
			}

			//probender->meshRenderComponent->GetBone("neck")->setInheritOrientation(false);

			//if(!boneName.empty() && //((RenderableJointType::RenderableJointType)i) == RenderableJointType::ShoulderRight ||
			//	//(((RenderableJointType::RenderableJointType)i) == RenderableJointType::WristRight) ||
			//	//(((RenderableJointType::RenderableJointType)i) == RenderableJointType::ElbowRight)||
			//	(((RenderableJointType::RenderableJointType)i) == RenderableJointType::SpineShoulder) )//||
			//	//(((RenderableJointType::RenderableJointType)i) == RenderableJointType::Neck))
			//{
			//	Ogre::Bone* currBone = probender->meshRenderComponent->GetBone(boneName);
			//	currBone->setManuallyControlled(true);
			//	currBone->setInheritOrientation(false);
			//	currBone->resetOrientation(); 
			//	
			//	currBone->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(180)), Ogre::Vector3(0.0f, 1.0f, 0.0f)) *
			//		//currBone->getParent()->_getFullTransform().extractQuaternion().Inverse() * 
			//		//currBone->convertWorldToLocalOrientation(
			//		
			//		jointWorldOrientations[i]);
			//	/*currBone->rotate(jointWorldOrientations[i]
			//	* Ogre::Quaternion(Ogre::Radian(Ogre::Degree(180)), Ogre::Vector3(0.0f, 1.0f, 0.0f)), Ogre::Node::TS_WORLD);*/
			//}
		}
	}

	//probender->meshRenderComponent->UpdateMesh(meshData, 0, Ogre::VES_POSITION);
}

void ProbenderInputHandler::SetBoneData(const std::string& boneName, const Ogre::Vector3& pos, bool inheritOrientation, 
										const Ogre::Quaternion& quat/* = Ogre::Quaternion::IDENTITY*/)
{
	Ogre::Bone* currBone = probender->meshRenderComponent->GetBone(boneName);
	currBone->setManuallyControlled(true);

	if(!inheritOrientation)
	{
		currBone->setInheritOrientation(false);
		currBone->resetOrientation(); 

		currBone->_setDerivedOrientation(quat);
	}
	else
	{
		if(quat != Ogre::Quaternion::IDENTITY)
		{
			currBone->resetOrientation();
			currBone->setOrientation(quat);
			
		}
	}
	
	//currBone->_setDerivedPosition(pos);

}

void ProbenderInputHandler::SetBoneData(const std::string& boneName, const bool updatePosition, 
		const bool updateOrientation, bool inheritOrientation /*= false*/, 
		const Ogre::Vector3& newPos /*= Ogre::Vector3(0.0f)*/, const Ogre::Quaternion& quat /*= Ogre::Quaternion::IDENTITY*/)
{
	Ogre::Bone* currBone = probender->meshRenderComponent->GetBone(boneName);
	currBone->setManuallyControlled(true);

	if(updateOrientation)
	{
		if(!inheritOrientation)
		{
			currBone->setInheritOrientation(false);
			currBone->resetOrientation(); 

			currBone->_setDerivedOrientation(quat);
		}
		else
		{
			if(quat != Ogre::Quaternion::IDENTITY)
			{
				currBone->resetOrientation();
				currBone->setOrientation(quat);

			}
		}
	}
	
	if(updatePosition)
	{
		//currBone->_setDerivedPosition(newPos);
	}
}

void ProbenderInputHandler::FillJointWorldOrientations(const CompleteData& currData)
{
	 Ogre::Quaternion rootQuat =  KinectVectorToOgreQuaternion(RenderableJointType::SpineBase, currData);
	 Ogre::Vector3 rootPos = KinectPosToOgrePosition(RenderableJointType::SpineBase, currData);

	SetBoneData("spine", false, true, false, Ogre::Vector3(0.0f),
		KinectVectorToOgreQuaternion(RenderableJointType::SpineMid, currData));

	SetBoneData("ribs", 
		probender->meshRenderComponent->GetBone("ribs")->getParent()->_getFullTransform().extractQuaternion() *
		(KinectPosToOgrePosition(RenderableJointType::SpineShoulder, currData) - rootPos),// * 1.5f,
		false,
		 probender->meshRenderComponent->GetBone("ribs")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::SpineShoulder, currData));
	//End of spine

	//Neck and Head
	SetBoneData("neck", false, true, false, Ogre::Vector3(0.0f), 
		probender->meshRenderComponent->GetBone("neck")->getParent()->_getDerivedOrientation() * 
		KinectVectorToOgreQuaternion(RenderableJointType::Neck, currData));

	SetBoneData("head", false, true, true, Ogre::Vector3(0.0f), 
		probender->meshRenderComponent->GetBone("head")->getParent()->_getDerivedOrientation() * 
		KinectVectorToOgreQuaternion(RenderableJointType::Neck, currData));
	
	//Right Arm
	//probender->meshRenderComponent->GetBone("upper_arm.R")->reset();//setInheritOrientation(false);
	SetBoneData("shoulder.R", 
		probender->meshRenderComponent->GetBone("shoulder.R")->getParent()->_getFullTransform().extractQuaternion() *
		((KinectPosToOgrePosition(RenderableJointType::ShoulderRight, currData)) - rootPos),
		false,
		probender->meshRenderComponent->GetBone("shoulder.R")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::ShoulderRight, currData));

	SetBoneData("upper_arm.R", false, true, false, Ogre::Vector3(0.0f),
		probender->meshRenderComponent->GetBone("upper_arm.R")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::ElbowRight, currData));

	SetBoneData("forearm.R", false, true, false, Ogre::Vector3(0.0f),
		probender->meshRenderComponent->GetBone("forearm.R")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::WristRight, currData));

	SetBoneData("hand.R", false, true, false, Ogre::Vector3(0.0f),
		probender->meshRenderComponent->GetBone("hand.R")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::WristRight, currData));
	//End of right arm

	//Left Arm
	SetBoneData("shoulder.L", 
		probender->meshRenderComponent->GetBone("shoulder.L")->getParent()->_getFullTransform().extractQuaternion() *
		((KinectPosToOgrePosition(RenderableJointType::ShoulderLeft, currData)) - rootPos),
		false,
		probender->meshRenderComponent->GetBone("shoulder.L")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::ShoulderLeft, currData));

	SetBoneData("upper_arm.L", false, true, false, Ogre::Vector3(0.0f),
		probender->meshRenderComponent->GetBone("upper_arm.L")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::ElbowLeft, currData));

	SetBoneData("forearm.L", false, true, false, Ogre::Vector3(0.0f),
		probender->meshRenderComponent->GetBone("forearm.L")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::WristLeft, currData));

	SetBoneData("hand.L", false, true, false, Ogre::Vector3(0.0f),
		probender->meshRenderComponent->GetBone("hand.L")->getParent()->_getFullTransform().extractQuaternion() * 
		KinectVectorToOgreQuaternion(RenderableJointType::WristLeft, currData));
	//End of Left arm

	//Right leg
	SetBoneData("thigh.R",
		KinectPosToOgrePosition(RenderableJointType::HipRight, currData) - rootPos,
		false,
		probender->meshRenderComponent->GetBone("thigh.R")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::KneeRight, currData));

	SetBoneData("shin.R",
		KinectPosToOgrePosition(RenderableJointType::KneeRight, currData) - rootPos,
		false,
		probender->meshRenderComponent->GetBone("shin.R")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::AnkleRight, currData) );

	SetBoneData("foot.R", false, true, true, KinectPosToOgrePosition(RenderableJointType::FootRight, currData) - rootPos, 
		probender->meshRenderComponent->GetBone("foot.R")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::AnkleRight, currData));

	//End of Right leg

	//Left leg
	SetBoneData("thigh.L",
		KinectPosToOgrePosition(RenderableJointType::HipLeft, currData) - rootPos,
		false,
		probender->meshRenderComponent->GetBone("thigh.L")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::KneeLeft, currData));

	SetBoneData("shin.L",
		KinectPosToOgrePosition(RenderableJointType::KneeLeft, currData) - rootPos,
		false,
		probender->meshRenderComponent->GetBone("shin.L")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::AnkleLeft, currData) );

	SetBoneData("foot.L", false, true, true, KinectPosToOgrePosition(RenderableJointType::FootRight, currData) - rootPos, 
		probender->meshRenderComponent->GetBone("foot.L")->getParent()->_getFullTransform().extractQuaternion() *
		KinectVectorToOgreQuaternion(RenderableJointType::AnkleLeft, currData));
	//End of Left leg

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

			if(currentData.LeanAmount.X < 0.0f)
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

						if(activeAttack)
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
	else
		activeAttack = NULL;
}

bool created = false;

void ProbenderInputHandler::DiscreteGesturesAcquired(const std::vector<KinectGestureResult>discreteGestureResults)
{
	AttackData frameData = AttackData();
	std::remove_const<const std::vector<KinectGestureResult>>::type noConstResults = discreteGestureResults;

	frameData.DiscreteGestureResults = &noConstResults;
	frameData._BodyDimensions = &bodyDimensions;
	frameData._Probender = probender;

	//HandleAttacks(frameData);
}

void ProbenderInputHandler::ContinuousGesturesAcquired(const std::vector<KinectGestureResult>continuousGestureResults)
{

}

#pragma endregion

#pragma region Speech Input

void ProbenderInputHandler::AudioDataReceived(AudioData* audioData)
{
	//if(audioData)
	//{
	//	std::wstring data = L"Data" + audioData->CommandValue;
	//	wprintf(data.c_str());
	//	std::wstring child = L"Child Data: " + audioData->ChildData->CommandValue;
	//	wprintf(child.c_str());

	//	std::wcout << L"Data: " << audioData->CommandValue.c_str() << std::endl;
	//	std::wcout << L"Child Data: " << audioData->ChildData->CommandValue.c_str() << std::endl;

	//	if(audioData->ChildData->CommandValue == L"AIR")
	//	{
	//		probender->SetCurrentElement(ElementEnum::Air);
	//	}
	//	else if(audioData->ChildData->CommandValue == L"EARTH")
	//	{
	//		probender->SetCurrentElement(ElementEnum::Earth);
	//	}
	//	else if(audioData->ChildData->CommandValue == L"FIRE")
	//	{
	//		probender->SetCurrentElement(ElementEnum::Fire);
	//	}
	//	else if(audioData->ChildData->CommandValue == L"WATER")
	//	{
	//		probender->SetCurrentElement(ElementEnum::Water);
	//	}
	//	//if(!reader->GetIsPaused())
	//	//	reader->Pause();
	//}
	//*quit = true;
}

#pragma endregion

#pragma region Mouse and Keyboard Input

void ProbenderInputHandler::keyDown(const OIS::KeyEvent &arg)
{
}

float angle = 0;

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
		//probender->Dodge(Probender::DD_LEFT);
		Ogre::Bone* bone = probender->meshRenderComponent->GetBone("head");

		Ogre::Vector3 bonePos = bone->_getDerivedPosition();

		printf("Bone Pos: %f, %f, %f\n", bonePos.x, bonePos.y, bonePos.z);

		bone->setManuallyControlled(true);
		bone->setInheritOrientation(false);

		bone->resetOrientation();

		Ogre::Quaternion boneOri = bone->getOrientation();
		Ogre::Quaternion boneDerived = bone->_getDerivedOrientation();

		printf("Bone Local Ori: %f, %f, %f, %f\n", boneOri.w, boneOri.x, boneOri.y, boneOri.z);

		printf("Bone Derived Ori: %f, %f, %f, %f\n", boneDerived.w, boneDerived.x, boneDerived.y, boneDerived.z);

		/*<rotation angle="1.811495">
		<axis x="0.051414" y="-0.884627" z="0.463456"/>*/

	}
	else if(arg.key == OIS::KC_RIGHT)
	{
		/*if(probender->rightHandAttack)
		{
		RigidBodyComponent* rigidBody = (RigidBodyComponent*)probender->rightHandAttack->GetComponent(Component::RIGID_BODY_COMPONENT);

		if(rigidBody)
		rigidBody->ApplyForce(physx::PxVec3(100.0f, 0.0f, 0.0f));
		}
		else*/
		{
			angle += 10;
			//probender->Dodge(Probender::DD_RIGHT);
			Ogre::Bone* bone = probender->meshRenderComponent->GetBone("head");

			Ogre::Vector3 bonePos = bone->_getDerivedPosition();

			printf("Bone Pos: %f, %f, %f\n", bonePos.x, bonePos.y, bonePos.z);

			bone->setManuallyControlled(true);
			bone->setInheritOrientation(false);
			bone->yaw(Ogre::Radian(Ogre::Degree(angle)), Ogre::Node::TS_LOCAL);
			//bone->rotate(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle)), Ogre::Vector3(1.0f, 0.0f, 0.0f)));
			//bone->setOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(angle)), Ogre::Vector3(0.0f ,1.0f, 0.0f)));
			//bone->setPosition(0.0f, 0.106f, 0.0f);

			printf(probender->meshRenderComponent->GetEntity()->getSkeleton()->getRootBone()->getName().c_str());

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
				CreateProjectile(ElementEnum::Fire, AbilityIDs::FIRE_BLAST);

			if(attack)
			{
				attack->CasterContestantID = probender->contestantID;

				attack->Enable();
				attack->GetRigidBody()->SetUseGravity(attack->GetUseGravity());

				Ogre::Vector3 camDir = probender->Forward();

				attack->SetWorldPosition((probender->GetWorldPosition() +
					probender->Forward() * 2.0f));
				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

				Ogre::Vector3 camForward = probender->camera->getDerivedOrientation() * -Ogre::Vector3::UNIT_Z;
				camForward.normalise();

				attack->SetWorldPosition(probender->GetWorldPosition()
					+ (camForward * attack->GetHalfExtents().x ));

				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

				//Launch the Projectile
				attack->LaunchProjectile(HelperFunctions::OgreToPhysXVec3(camForward), 
					probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
					probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());
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

				/*Ogre::Vector3 camDir = probender->Forward();

				attack->SetWorldPosition(probender->GetWorldPosition() + 
					(probender->Forward() * (renderComp->GetHalfExtents().x * 2)));*/

				Ogre::Vector3 camForward = probender->camera->getDerivedOrientation() * -Ogre::Vector3::UNIT_Z;
				camForward.normalise();

				attack->SetWorldPosition(probender->GetWorldPosition()
					+ (camForward * attack->GetHalfExtents().x ));

				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);
				
				//Launch the Projectile
				attack->LaunchProjectile(HelperFunctions::OgreToPhysXVec3(camForward), 
					probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
					probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());
/*
				attack->LaunchProjectile(physx::PxVec3(camDir.x, camDir.y, camDir.z), 
					probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
					probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());*/

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
				//attack->GetRigidBody()->SetUseGravity(true);

				MeshRenderComponent* renderComp = (MeshRenderComponent*)attack->GetComponent(Component::MESH_RENDER_COMPONENT);

				//Ogre::Vector3 camDir = probender->Forward();
				Ogre::Vector3 camForward = probender->camera->getDerivedOrientation() * -Ogre::Vector3::UNIT_Z;
				camForward.normalise();

				attack->SetWorldPosition(probender->GetWorldPosition()
					+ (camForward * attack->GetHalfExtents().x ));

				/*attack->SetWorldPosition(probender->GetWorldPosition() + 
					(probender->Forward() * (renderComp->GetHalfExtents().x * 2)));*/
				attack->SetWorldOrientation(1.0f, 0.0f, 0.0f, 0.0f);

				//Launch the Projectile
				attack->LaunchProjectile(HelperFunctions::OgreToPhysXVec3(camForward), 
					probender->GetInGameData().CurrentAttributes.GetBonusAttackSpeed(),
					probender->GetInGameData().CurrentAttributes.GetBonusAttackDamage());
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