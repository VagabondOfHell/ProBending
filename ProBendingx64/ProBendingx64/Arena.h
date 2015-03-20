#pragma once
#include "ArenaData.h"

#include "GameObject.h"

#include <map>
#include <vector>

class IScene;
class Probender;
struct ProbenderData;
class ProjectileManager;

class Arena
{
	friend class ArenaBuilder;

protected:
	//Blue Zone 3 because 0 == Invalid. So Blue Zone 3 represents count
	SharedGameObject zoneStartPositions[ArenaData::BLUE_ZONE_3];

	std::string arenaName;//Name of this arena
	std::string resourceGroupName;//Name of the resource group that this arena uses

	bool loaded;//true if resources have already been loaded for this arena

	IScene* owningScene;//The scene that owns the arena

	ProjectileManager* projectileManager;//The manager of the projectiles in the scene

	unsigned short contestantCount;//Number of Probenders in the arena

	std::vector<std::shared_ptr<Probender>> contestants;//Array of Probenders

	virtual void PlaceContestants();

	ArenaData::Zones GetAdjacentZone(ArenaData::Zones currentZone, bool towardsCenter);

public:
	Arena(IScene* _owningScene, std::string _arenaName);

	virtual ~Arena(void);

	inline IScene* GetOwningScene()const{return owningScene;}

	///<summary>Gets the name associated with the arena</summary>
	///<returns>The name of the arena</returns>
	inline std::string GetArenaName()const {return arenaName;}

	///<summary>Gets the name of the resource group that contains this arena's render resources</summary>
	///<returns>The string representing the group resource name</returns>
	inline std::string GetResourceGroupName()const{return resourceGroupName;}

	///<summary>Gets the projectile manager of the scene</summary>
	///<returns>A pointer to the projectile manager, or NULL if not initialized yet</returns>
	inline ProjectileManager* const GetProjectileManager()const{return projectileManager;}
	
	virtual void Initialize(const std::vector<ProbenderData> contestantData);
	
	virtual bool SavePhysXData(const std::string& fileName, const std::string& collectionName);
	
	virtual bool LoadPhysXData(const std::string& fileName, const std::string& collectionName);

	void BeginTransition(unsigned short contestantID, ArenaData::Zones newZone, ArenaData::Zones oldZone);

	virtual void Start();

	virtual bool Update(const float gameTime);

	virtual bool SerializeArena();

	virtual bool DeserializeArena();

};

