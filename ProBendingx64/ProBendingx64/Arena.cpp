
#include "OgreSceneNode.h"
#include "OgreCamera.h"

#include "Arena.h"
#include "Probender.h"
#include "GUIManager.h"
#include "GameScene.h"

#include "ProjectileManager.h"
#include "PhysXDataManager.h"
#include "PhysXSerializerWrapper.h"
#include "SceneSerializer.h"
#include "XMLReader.h"

Arena::Arena(IScene* _owningScene, std::string _arenaName)
	:loaded(false), arenaName(_arenaName)
{
	owningScene = _owningScene;
	resourceGroupName = arenaName + "Resources";
	resourceGroupName.erase(std::remove_if(resourceGroupName.begin(), resourceGroupName.end(), isspace), resourceGroupName.end());
	Probender::CreateContestantMeshes(owningScene->GetOgreSceneManager(), true, true, true, true, true, true);
}

Arena::~Arena(void)
{
	if(projectileManager)
		delete projectileManager;
}

void Arena::Initialize(const std::vector<ProbenderData> contestantData)
{
	
	projectileManager = new ProjectileManager(owningScene);

	contestantCount = (unsigned short)contestantData.size();
	contestants.reserve(contestantData.size());
	
	//Loop and initialize each character
	for (int i = 0; i < contestantCount; i++)
	{
		contestants.push_back(std::make_shared<Probender>(i, contestantData[i], this));
	}
}

bool Arena::SavePhysXData(const std::string& fileName, const std::string& collectionName)
{
	bool success = false;

	PhysXSerializerWrapper::CreateSerializer();

	PxDataManSerializeOptions pxDataManOptions = PxDataManSerializeOptions(PxDataManSerializeOptions::ALL,
		collectionName, true, fileName);

	std::string displayMessage = "PhysX Data for " + arenaName;
	if(PhysXDataManager::GetSingletonPtr()->SerializeData(pxDataManOptions))
	{
		displayMessage += " Serialization successful\n";
		printf(displayMessage.c_str());
		success = true;
	}
	else
	{
		displayMessage += " Serialization unsuccessful\n";
		printf(displayMessage.c_str());
	}

	PhysXSerializerWrapper::DestroySerializer();
	return success;

}

bool Arena::LoadPhysXData(const std::string& fileName, const std::string& collectionName)
{
	bool success = false;

	PhysXSerializerWrapper::CreateSerializer();
	
	PxDataManSerializeOptions pxDataManOptions = PxDataManSerializeOptions(PxDataManSerializeOptions::ALL,
		collectionName, true, fileName);

	std::string displayMessage = "PhysX Data for " + arenaName;
		
	if(PhysXDataManager::GetSingletonPtr()->DeserializeData(pxDataManOptions))
	{
		displayMessage += " Deserialized successfully\n";
		printf(displayMessage.c_str());

		success = PhysXSerializerWrapper::AddToScene(owningScene->GetPhysXScene(), collectionName);
	}
	else
	{
		displayMessage += " Deserialized unsuccessfully\n";
		printf(displayMessage.c_str());
	}
			
	PhysXSerializerWrapper::DestroySerializer();

	return success;
}

void Arena::BeginTransition(unsigned short contestantID, ArenaData::Zones newZone)
{
	if(newZone == ArenaData::INVALID_ZONE)
		return;

	for (int i = 0; i < contestants.size(); i++)
	{
		contestants[i]->TransitionToPoint(
			HelperFunctions::OgreToPhysXVec3(zoneStartPositions[newZone - 1]->GetWorldPosition()));
	}
}

void Arena::Start()
{
	PlaceContestants();

	short earthCount, fireCount, waterCount, airCount;
	earthCount = fireCount = waterCount = airCount = 0;

	//Loop and initialize each character
	for (int i = 0; i < contestantCount; i++)
	{
		switch (contestants[i]->GetInGameData().MainElement)
		{
		case ElementEnum::Earth:
			earthCount++;
			break;
		case ElementEnum::Air:
			airCount++;
			break;
		case ElementEnum::Fire:
			fireCount++;
			break;
		case ElementEnum::Water:
			waterCount++;
			break;
		default:
			break;
		}
	}

	projectileManager->CreatePool(ElementEnum::Earth, earthCount);
	projectileManager->CreatePool(ElementEnum::Fire, fireCount);
	projectileManager->CreatePool(ElementEnum::Air, airCount);
	projectileManager->CreatePool(ElementEnum::Water, waterCount);
}

void Arena::PlaceContestants()
{
	//Find the start positions for each colours zone
	zoneStartPositions[ArenaData::RED_ZONE_1 - 1] = owningScene->FindByName("RedZone1SP");
	zoneStartPositions[ArenaData::RED_ZONE_2 - 1] = owningScene->FindByName("RedZone2SP");
	zoneStartPositions[ArenaData::RED_ZONE_3 - 1] = owningScene->FindByName("RedZone3SP");
	zoneStartPositions[ArenaData::BLUE_ZONE_1 - 1] = owningScene->FindByName("BlueZone1SP");
	zoneStartPositions[ArenaData::BLUE_ZONE_2 - 1] = owningScene->FindByName("BlueZone2SP");
	zoneStartPositions[ArenaData::BLUE_ZONE_3 - 1] = owningScene->FindByName("BlueZone3SP");
	
	Ogre::Vector3 rightShift(0.0f, 0.0f, 1.0f);
	Ogre::Vector3 leftShift(0.0f, 0.0f, -1.0f);

	//Count of how many contestants are placed within each zone
	int rz1, rz2, rz3;
	int bz1, bz2, bz3;
	rz1 = rz2 = rz3 = bz1 = bz2 = bz3 = 0;

	for (int i = 0; i < contestantCount; ++i)
	{
		contestants[i]->Start();

		//For debugging purposes
		if(i ==1)
			contestants[i]->SetKeyboardConfiguration(ConfigurationLayout(OIS::KC_Q, OIS::KC_TAB, OIS::KC_2));
		//End debugging purposes

		ArenaData::Zones currZone = contestants[i]->GetCurrentZone();
		ArenaData::Team currTeam = contestants[i]->GetTeam();

		//if invalid zone, set to first zone of corresponding team
		if(currZone == ArenaData::INVALID_ZONE)
			currZone = currTeam == ArenaData::BLUE_TEAM ? ArenaData::BLUE_ZONE_1 : ArenaData::RED_ZONE_1;

		Ogre::Vector3 currZoneWorldPosition;
		Ogre::Vector3 currShift;
		
		unsigned int shiftIndex;

		//Position game characters in accordance to starting zone. Shift left or right of the zones' start position
		//based on the number of characters already assigned to that zone (allows customization of handicaps later on)
		switch (currZone)
		{
		case ArenaData::RED_ZONE_1:
			currZoneWorldPosition = zoneStartPositions[ArenaData::RED_ZONE_1 - 1]->GetWorldPosition();
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90.0f)), Ogre::Vector3::UNIT_Y));
			shiftIndex = rz1;
			++rz1;
			break;
		case ArenaData::RED_ZONE_2:
			currZoneWorldPosition = zoneStartPositions[ArenaData::RED_ZONE_2 - 1]->GetWorldPosition();
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90.0f)), Ogre::Vector3::UNIT_Y));
			shiftIndex = rz2;
			++rz2;
			break;
		case ArenaData::RED_ZONE_3:
			currZoneWorldPosition = zoneStartPositions[ArenaData::RED_ZONE_3 - 1]->GetWorldPosition();
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90.0f)), Ogre::Vector3::UNIT_Y));
			shiftIndex = rz3;
			++rz3;
			break;
		case ArenaData::BLUE_ZONE_1:
			currZoneWorldPosition = zoneStartPositions[ArenaData::BLUE_ZONE_1 - 1]->GetWorldPosition();
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_Y));
			shiftIndex = bz1;
			++bz1;
			break;
		case ArenaData::BLUE_ZONE_2:
			currZoneWorldPosition = zoneStartPositions[ArenaData::BLUE_ZONE_2 - 1]->GetWorldPosition();
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_Y));
			shiftIndex = bz2;
			++bz2;
			break;
		case ArenaData::BLUE_ZONE_3:
			currZoneWorldPosition = zoneStartPositions[ArenaData::BLUE_ZONE_3 - 1]->GetWorldPosition();
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_Y));
			shiftIndex = bz3;
			++bz3;
			break;
		}

		if(shiftIndex == 0)
			currShift = Ogre::Vector3::ZERO;
		else if(shiftIndex == 1)
			currShift = leftShift;
		else if(shiftIndex == 2)
			currShift = rightShift;

		contestants[i]->SetWorldPosition(currZoneWorldPosition + currShift);
		owningScene->AddGameObject(contestants[i]);
	}

	contestants[0]->SetCamera(owningScene->GetCamera());
	contestants[1]->SetCamera(((GameScene*)owningScene)->GetCamera2());

	contestants[0]->currentTarget = contestants[1].get();
	contestants[1]->currentTarget = contestants[0].get();
}

bool Arena::Update(const float gameTime)
{
	GameScene* gameScene = (GameScene*)owningScene;

	if(gameScene->GetCurrentState() == GameScene::GS_TRANSITION)
	{
		bool transitioning = false;

		for (int i = 0; i < contestants.size(); i++)
		{
			if(contestants[i]->stateManager.GetCurrentState() == StateFlags::TRANSITION_STATE)
			{
				transitioning = true;
				break;
			}
		}

		if(!transitioning)//if no more transitions, indicate all contestants should listen
		{
			for (int i = 0; i < contestants.size(); i++)
			{
				contestants[i]->SetInputState(Probender::Listen);
			}
		}
	}

	//Contestants get updated by the game object list
	//Update the projectile manager
	projectileManager->Update(gameTime);
	
	return true;
}

bool Arena::SerializeArena()
{
	bool success = false;

	PhysXSerializerWrapper::CreateSerializer();
	
	std::string arenaFileName =PxDataManSerializeOptions::DEFAULT_FILE_PATH + arenaName + "\\" + arenaName;
	arenaFileName.erase(std::remove_if(arenaFileName.begin(), arenaFileName.end(), isspace), arenaFileName.end());

	if(SavePhysXData(arenaFileName, "ArenaCollection"))
	{
		std::string displayMessage = "XML Scene Serialization for " + arenaName;

		SceneSerializer sceneSerializer = SceneSerializer();
		if(sceneSerializer.SerializeScene(owningScene, arenaName, arenaFileName))
		{
			displayMessage += " successful\n";
			printf(displayMessage.c_str());

			success = true;
		}
		else
		{
			displayMessage += " unsuccessful\n";
			printf(displayMessage.c_str());
		}

	}

	PhysXSerializerWrapper::DestroySerializer();

	return success;
}

bool Arena::DeserializeArena()
{
	bool success = false;

	PhysXSerializerWrapper::CreateSerializer();

	std::string arenaFileName =PxDataManSerializeOptions::DEFAULT_FILE_PATH + arenaName + "\\" + arenaName;
	arenaFileName.erase(std::remove_if(arenaFileName.begin(), arenaFileName.end(), isspace), arenaFileName.end());

	if(LoadPhysXData(arenaFileName, "ArenaCollection"))
	{
		std::string displayMessage = "XML Scene Deserialization for " + arenaName;

		SceneSerializer sceneSerializer = SceneSerializer();
		if(sceneSerializer.DeserializeScene(owningScene, arenaFileName, "ArenaCollection"))
		{
			displayMessage += " successful\n";
			printf(displayMessage.c_str());

			success = true;
		}
		else
		{
			displayMessage += " unsuccessful\n";
			printf(displayMessage.c_str());
		}
	}

	PhysXSerializerWrapper::DestroySerializer();
	return success;
}


