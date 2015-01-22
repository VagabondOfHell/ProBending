#pragma once
#define NOMINMAX
#include "foundation\PxVec3.h"
#include "PxSceneDesc.h"
#include "OgreVector3.h"

#include "GameObject.h"

#include <string>
#include <unordered_set>
#include <memory>

namespace Ogre
{
	class SceneManager;
	class Camera;
	class Root;
	class SceneNode;
	class ColourValue;
};

namespace physx
{
	class PxCudaContextManager;
	class PxDefaultCpuDispatcher;
	class PxScene;
};

class SceneManager;
class GUIManager;

typedef std::shared_ptr<GameObject> SharedGameObject;

class IScene
{
	friend class SceneSerializer;

private:
	typedef std::unordered_set<SharedGameObject> GameObjectList;
	GameObjectList gameObjectList;

protected:
	SceneManager* owningManager;

	std::string resourceGroupName;
	
	Ogre::SceneManager* ogreSceneManager;

	Ogre::Camera* mainOgreCamera;

	GUIManager* guiManager;

	physx::PxScene* physicsWorld;
	bool physxSimulating;

	bool physxEnabled;

	physx::PxDefaultCpuDispatcher* mCpuDispatcher;
	
	//Cuda Manager for GPU Processor usage
	physx::PxCudaContextManager* cudaContextManager;

	bool started;

	inline void SetResourceGroupName(std::string _resourceGroupName)
	{
		resourceGroupName = _resourceGroupName;
	}

	///<summary>Creates the cuda context</summary>
	///<returns>True if successful, false if not</returns>
	bool CreateCudaContext();

	///<summary>Helper method for basic scene descriptors</summary>
	///<returns>Returns a scene description object</returns>
	physx::PxSceneDesc* GetDefaultSceneDescription(physx::PxVec3& gravity, bool initializeCuda);

	///<summary>Childs opportunity to create custom scene descriptions</summary>
	///<param name="gravity">The passed in gravity value</param>
	///<param name="initializeCuda">True to use CUDA, false if not</param>
	///<returns>Pointer to a scene description for PhysX to construct its scene</returns>
	virtual physx::PxSceneDesc* GetSceneDescription(physx::PxVec3& gravity, bool initializeCuda);
public:
	IScene(SceneManager* _owningManager, Ogre::Root* root, std::string _sceneName, std::string _resourceGroupName);

	virtual ~IScene();	

	///<summary>Handles creation of the viewport and the Camera</summary>
	///<param name="clearColour">The colour to clear the viewport with</param>
	///<param name="camPos">The start position of the camera</param>
	///<param name="lookAt">Where the camera should look towards</param>
	///<param name="nearClip">The near clipping plane</param>
	///<param name="farClip">The far clipping plane</param>
	virtual void CreateCameraAndViewport(const Ogre::ColourValue& clearColour, const Ogre::Vector3& camPos = Ogre::Vector3(0.0f), 
		const Ogre::Vector3& lookAt = Ogre::Vector3(0.0f), float nearClip = 0.1f, float farClip = 10000.0f);

	virtual void Initialize();

	///<summary>Initializes PhysX Physics with the scene, handling generic PxScene creation. Marked virtual for more
	///customization in other scenes if necessary</summary>
	///<param name="gravity">The gravity force acted on all objects in the physics scene</param>
	///<param name="initializeCuda">True to initialize the CUDA component of physx, false if not</param>
	void InitializePhysics(physx::PxVec3& gravity = physx::PxVec3(0.0f, -9.8f, 0.0f), bool initializeCuda = false);

	///<summary>Creates a new game object</summary>
	///<param name="objectName">The name to assign to the object</param>
	///<returns>Pointer to the newly created Game Object</returns>
	inline SharedGameObject CreateGameObject(const std::string& objectName = "")
	{
		SharedGameObject newObject = std::make_shared<GameObject>(GameObject(this, objectName));
		gameObjectList.insert(newObject);
		return newObject;
	}

	///<summary>Adds a game object (used for inherited types)</summary>
	///<param name="gameObject">The shared pointer to the game object</param>
	///<returns>True if addable, false if not</returns>
	inline bool AddGameObject(SharedGameObject gameObject)
	{
		std::pair<GameObjectList::iterator, bool> result = gameObjectList.insert(gameObject);
		return result.second;
	}

	///<summary>Destroys the specified game object</summary>
	///<param name="gameObject">The game object to destroy</param>
	///<returns></returns>
	inline void RemoveGameObject(SharedGameObject gameObject)
	{
		gameObjectList.erase(gameObject);
	}

	///<summary>Called by the scene manager when the scene is first started</summary>
	virtual void Start() = 0;

	///<summary>Called by the scene manager every frame. This is where updating takes place</summary>
	///<param name="gameTime">The time passed between frames</param>
	virtual bool Update(float gameTime) = 0;

	///<summary>Called before the destructor to handle any necessary cleanup beforehand</summary>
	virtual void Close() = 0;

	///<summary>Gets the main Ogre Camera of the scene</summary>
	///<returns>The Ogre Camera, or NULL if none have been assigned yet</returns>
	inline Ogre::Camera*const GetCamera()const{	return mainOgreCamera;	}
	
	///<summary>Gets whether or not the scene has been started yet</summary>
	///<returns>True if started, false if not</returns>
	inline const bool GetIsStarted()const{return started;}

	///<summary>Gets the resource group associated with this scene. Child classes can potentially have more than
	///one of these, but its up to them to provide accessors</summary>
	///<returns>String representing the resource group name, or empty string if none</returns>
	inline const std::string GetResourceGroupName()const{return resourceGroupName;}

	///<summary>Gets the name of the scene</summary>
	///<returns>String representing the name of the scene, or empty string if none assigned</returns>
	std::string GetSceneName()const;
	
	///<summary>Checks if this scene is utilizing physx for physics</summary>
	///<returns>True if physx has been initialized, false if not</returns>
	inline const bool IsUsingPhysX()const{return physxEnabled;}

	///<summary>Gets the root scene node of the Ogre Scene Manager</summary>
	///<returns>The Ogre Scene Node used as the Root</returns>
	Ogre::SceneNode* GetSceneRootNode()const;

	///<summary>Returns the Ogre Scene Manager associated with this scene</summary>
	///<returns>The Ogre Scene Manager</returns>
	inline Ogre::SceneManager* GetOgreSceneManager()const{return ogreSceneManager;}

	///<summary>Gets the Cuda Context Manager associated with this scene</summary>
	///<returns>The Cuda Context Manager, or NULL if Cuda wasn't initialized</returns>
	inline physx::PxCudaContextManager* const GetCudaContextManager()const{	return cudaContextManager;}

	///<summary>Gets the PhysX Scene associated with this scene</summary>
	///<returns>The PxScene, or NULL if physx wasn't initialized</returns>
	inline physx::PxScene* const GetPhysXScene()const{return physicsWorld;}

	///<summary>Gets the GUI Manager associated with the scene</summary>
	///<returns>The GUI Manager, or NULL if the scene hasn't been initialized yet</returns>
	inline GUIManager* const GetGUIManager()const{return guiManager;}

	///<summary>Initializes the resources of the specified group name</summary>
	///<param name="resourceName">The name of the resource group to be initialized</param>
	virtual void InitializeResources(const std::string& resourceGroupName);

	///<summary>This method unloads all resources in the group, but in addition it removes all those 
	///resources from their ResourceManagers, and then clears all the members from the list. 
	///That means after calling this method, there are no resources declared as part of the 
	///named group any more. Resource locations still persist though. </summary>
	///<param name="resourceName">The name of the resource group to be cleared</param>
	virtual void ClearResources(const std::string& resourceGroupName);

	///<summary> This method unloads all the resources that have been declared as
    ///being part of the named resource group. Note that these resources
    ///will still exist in their respective ResourceManager classes, but
    ///will be in an unloaded state. If you want to remove them entirely,
    ///you should use clearResourceGroup or destroyResourceGroup.</summary>
	///<param name="resourceName">The name of the resource group to be unloaded</param>
	virtual void UnloadResources(const std::string& resourceGroupName);

	///<summary> Destroys a resource group, clearing it first, destroying the resources
    ///which are part of it, and then removing it from
    ///the list of resource groups. </summary>
	///<param name="resourceName">The name of the resource group to be destroyed</param>
	virtual void DestroyResources(const std::string& resourceGroupName);

};