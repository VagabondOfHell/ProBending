
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

	using namespace CEGUI;

	label = owningScene->GetGUIManager()->CreateGUIButton("TaharezLook/Button", "ElementDisplay", 
		ElementEnum::EnumToString(contestants[0]->GetInGameData().MainElement), 
		UVector2(UDim(0.0f, 0.0f), UDim(0.9f, 0.0f)), USize(UDim(0.3f, 0.0f), UDim(0.05f, 0.0f)));
}

void Arena::PlaceContestants()
{
	//Find the start positions for each colours zone
	SharedGameObject rz1Obj = owningScene->FindByName("RedZone1SP");
	SharedGameObject rz2Obj = owningScene->FindByName("RedZone2SP");
	SharedGameObject rz3Obj = owningScene->FindByName("RedZone3SP");
	SharedGameObject bz1Obj = owningScene->FindByName("BlueZone1SP");
	SharedGameObject bz2Obj = owningScene->FindByName("BlueZone2SP");
	SharedGameObject bz3Obj = owningScene->FindByName("BlueZone3SP");

	if(!rz1Obj)
		return;

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

		//Position game characters in accordance to starting zone. Shift left or right of the zones' start position
		//based on the number of characters already assigned to that zone (allows customization of handicaps later on)
		switch (currZone)
		{
		case ArenaData::RED_ZONE_1:
			if(rz1 == 0)
				contestants[i]->SetWorldPosition(rz1Obj->GetWorldPosition());
			else if(rz1 == 1)
				contestants[i]->SetWorldPosition(rz1Obj->GetWorldPosition() + leftShift);
			else if(rz1 == 2)
				contestants[i]->SetWorldPosition(rz1Obj->GetWorldPosition() + rightShift);
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90.0f)), Ogre::Vector3::UNIT_Y));
			++rz1;
			break;
		case ArenaData::RED_ZONE_2:
			if(rz2 == 0)
				contestants[i]->SetWorldPosition(rz2Obj->GetWorldPosition());
			else if(rz2 == 1)
				contestants[i]->SetWorldPosition(rz2Obj->GetWorldPosition() + leftShift);
			else if(rz2 == 2)
				contestants[i]->SetWorldPosition(rz2Obj->GetWorldPosition() + rightShift);
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90.0f)), Ogre::Vector3::UNIT_Y));
			++rz2;
			break;
		case ArenaData::RED_ZONE_3:
			if(rz3 == 0)
				contestants[i]->SetWorldPosition(rz3Obj->GetWorldPosition());
			else if(rz3 == 1)
				contestants[i]->SetWorldPosition(rz3Obj->GetWorldPosition() + leftShift);
			else if(rz3 == 2)
				contestants[i]->SetWorldPosition(rz3Obj->GetWorldPosition() + rightShift);
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(-90.0f)), Ogre::Vector3::UNIT_Y));
			++rz3;
			break;
		case ArenaData::BLUE_ZONE_1:
			if(bz1 == 0)
				contestants[i]->SetWorldPosition(bz1Obj->GetWorldPosition());
			else if(bz1 == 1)
				contestants[i]->SetWorldPosition(bz1Obj->GetWorldPosition() + leftShift);
			else if(bz1 == 2)
				contestants[i]->SetWorldPosition(bz1Obj->GetWorldPosition() + rightShift);
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_Y));
			++bz1;
			break;
		case ArenaData::BLUE_ZONE_2:
			if(bz2 == 0)
				contestants[i]->SetWorldPosition(bz2Obj->GetWorldPosition());
			else if(bz2 == 1)
				contestants[i]->SetWorldPosition(bz2Obj->GetWorldPosition() + leftShift);
			else if(bz2 == 2)
				contestants[i]->SetWorldPosition(bz2Obj->GetWorldPosition() + rightShift);
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_Y));
			++bz2;
			break;
		case ArenaData::BLUE_ZONE_3:
			if(bz3 == 0)
				contestants[i]->SetWorldPosition(bz3Obj->GetWorldPosition());
			else if(bz3 == 1)
				contestants[i]->SetWorldPosition(bz3Obj->GetWorldPosition() + leftShift);
			else if(bz3 == 2)
				contestants[i]->SetWorldPosition(bz3Obj->GetWorldPosition() + rightShift);
			contestants[i]->SetWorldOrientation(Ogre::Quaternion(Ogre::Radian(Ogre::Degree(90.0f)), Ogre::Vector3::UNIT_Y));
			++bz3;
			break;
		}

		owningScene->AddGameObject(contestants[i]);
	}

	contestants[0]->SetCamera(owningScene->GetCamera());
	contestants[1]->SetCamera(((GameScene*)owningScene)->GetCamera2());

	contestants[0]->currentTarget = contestants[1].get();
	contestants[1]->currentTarget = contestants[0].get();
}

bool Arena::Update(const float gameTime)
{
	/*for (int i = 0; i < contestantCount; i++)
	{
		contestants[i]->Update(gameTime);
	}*/

	label->setText(std::to_string(contestants[0]->GetInGameData().CurrentAttributes.Energy));
	
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

