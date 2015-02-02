#pragma once
#include <map>
#include <vector>
#include "CEGUI/widgets/PushButton.h"

class IScene;
class GameObject;
class Probender;
struct ProbenderData;
class AbilityManager;
class ProjectileManager;

class Arena
{
	friend class ArenaBuilder;

protected:
	std::string arenaName;//Name of this arena
	std::string resourceGroupName;//Name of the resource group that this arena uses

	bool loaded;//true if resources have already been loaded for this arena

	IScene* owningScene;//The scene that owns the arena
	AbilityManager* abilityManager;//The manager of all abilities in the scene
	ProjectileManager* projectileManager;//The manager of the projectiles in the scene

	unsigned short contestantCount;//Number of Probenders in the arena

	std::map<std::string, GameObject*> staticArenaObjects;//Map of arena objects that don't move
	std::map<std::string, GameObject*> dynamicArenaObjects;//Map of arena objects that can move

	Probender* contestants;//Array of Probenders

	CEGUI::PushButton* label;//For prototype demo

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

	///<summary>Gets the ability manager that oversees the current games' abilities</summary>
	///<returns>Pointer to the ability manager</returns>
	inline AbilityManager* const GetAbilityManager()const{return abilityManager;}

	///<summary>Gets the projectile manager of the scene</summary>
	///<returns>A pointer to the projectile manager, or NULL if not initialized yet</returns>
	inline ProjectileManager* const GetProjectileManager()const{return projectileManager;}

	virtual void Initialize(const std::vector<ProbenderData> contestantData);

	virtual bool SavePhysXData(const std::string& fileName, const std::string& collectionName);

	virtual bool LoadResources();

	virtual bool LoadPhysXData(const std::string& fileName, const std::string& collectionName);

	virtual void Start();

	virtual bool Update(const float gameTime);

	virtual bool SerializeArena();

	virtual bool DeserializeArena();

};

