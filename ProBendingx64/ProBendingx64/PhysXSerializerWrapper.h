#pragma once
#include "common/PxBase.h"
#include "foundation/PxSimpleTypes.h"
#include "foundation/PxIO.h"
#include <map>
#include <vector>
#include <string>

namespace physx
{
	class PxCollection;
	class PxSerializationRegistry;
	class PxScene;
};

class PhysXSerializerWrapper
{
private:
	static unsigned int ReferenceCount;

	static physx::PxCollection* workingCollection;

	typedef std::map<std::string, physx::PxCollection*> CollectionMap;
	typedef std::pair<CollectionMap::iterator, bool> CollectionInsertionResult;

	static CollectionMap collectionMap;
	static physx::PxSerializationRegistry* serializationRegistry;

	static std::vector<physx::PxU8*> dynamicMemoryList;

	inline static bool FindExistingWithHint(const std::string& nameToFind, CollectionMap::iterator& outVal)
	{
		outVal = collectionMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != collectionMap.end() && !(collectionMap.key_comp()(nameToFind, outVal->first)));
	}

	///<summary>Creates and returns a collection under the specified name</summary>
	///<param name="collectionName">The name to give to the collection</param>
	///<returns>The new collection, or NULL if it already exists or invalid Name</returns>
	static physx::PxCollection* CreateAndGetCollection(const std::string& collectionName);

	///<summary>Sets the collection to perform calls on</summary>
	///<param name="collection">The collection to make the current working collection</param>
	///<returns>True if successful, false if collection is NULL</returns>
	static inline bool SetWorkingCollection(physx::PxCollection* collection)
	{
		if(collection)
			workingCollection = collection;

		return workingCollection != NULL;
	}

	static inline void* CreateAlignedBlock(physx::PxInputData& data, physx::PxU8*& baseAddr)
	{
		physx::PxU32 length = data.getLength();
		baseAddr = (physx::PxU8*)malloc(length+PX_SERIAL_FILE_ALIGN-1);
		void* alignedBlock = (void*)((size_t(baseAddr)+PX_SERIAL_FILE_ALIGN-1)&~(PX_SERIAL_FILE_ALIGN-1));
		data.read(alignedBlock, length);
		return alignedBlock;
	}

public:

	///<summary>Creates the Serializer using the PhysX Extension
	///This wrapper is reference counted. This allows create to be called multiple
	///times without problem. However, every call to Create needs to be matched with Destroy</summary>
	///<returns>True if successful, false if already created or failure</returns>
	static bool CreateSerializer();

	///<summary>Releases the Serializer extension. Also releases all collections
	///Decrements the reference counter</summary>
	static void DestroySerializer();

	///<summary>Called when the SDK is closing to release objects loaded through serialization</summary>
	static void CleanMemory()
	{
		for (auto start = dynamicMemoryList.begin(); start != dynamicMemoryList.end(); ++start)
		{
			if(*start)
			{
				delete[] *start;
				*start = NULL;
			}
			
		}

		dynamicMemoryList.clear();
	}

	///<summary>Creates a new collection under the specified name</summary>
	///<param name="collectionName">The name to store the collection under. Must be Unique</param>
	///<returns>True if successful, false if blank name, name exists, or creation fails</returns>
	static inline bool CreateCollection(const std::string& collectionName, const bool setWorkingCollection = false)
	{
		physx::PxCollection* collection = CreateAndGetCollection(collectionName);
		
		if(setWorkingCollection)
			return SetWorkingCollection(collection); //return result of assigning the collection

		return collection != NULL; //return successful creation
	}

	static inline bool AddCollection(const std::string& collectionName, physx::PxCollection* collectionToAdd)
	{
		if(collectionName.empty())//Invalid name
			return false;

		CollectionMap::iterator hint = collectionMap.begin();

		if(FindExistingWithHint(collectionName, hint ))//if collection exists with that name
			return false;//indicate insertion failure

		//insert and return true
		collectionMap.insert(hint, CollectionMap::value_type(collectionName, collectionToAdd));
		
		return true;
	}

	///<summary>Sets the collection that should be currently worked on</summary>
	///<param name="collectionName">The name of the collection to find and assign</param>
	///<returns>True if successful, false if name wasnt found</returns>
	static bool SetWorkingCollection(const std::string& collectionName);

	///<summary>Adds a physX object to the collection to be serialized</summary>
	///<param name="collectionName">The name of the collection to add to</param>
	///<param name="objectToAdd">The object to add to the collection</param>
	///<param name="objectID">The optional ID to store the collection under. 
	///Must be Unique to the collection. Used to connect references</param>
	///<returns>True if successful, false if object exists in collection or ID exists</returns>
	static bool AddToCollection(const std::string& collectionName, physx::PxBase& objectToAdd, 
		physx::PxSerialObjectId objectID = PX_SERIAL_OBJECT_ID_INVALID);

	///<summary>Adds an object to the collection currently set to be worked on with 
	///SetWorkingCollection. </summary>
	///<param name="objectToAdd">The object to add to the collection</param>
	///<param name="objectID">The ID to assign to the object</param>
	///<returns>True if successful, false if not</returns>
	static bool AddToWorkingCollection(physx::PxBase& objectToAdd, 
		physx::PxSerialObjectId objectID = PX_SERIAL_OBJECT_ID_INVALID);

	///<summary>Clears the collection set as the current working collection</summary>
	static inline void ClearWorkingCollection()
	{
		workingCollection = NULL;
	}

	///<summary>Gets the physx collection of the associated name</summary>
	///<param name="collectionName">The name of the collection to retrieve</param>
	///<returns>The collection if found, NULL if not</returns>
	static inline physx::PxCollection* GetCollection(const std::string& collectionName)
	{
		CollectionMap::iterator findResult = collectionMap.find(collectionName);

		if(findResult != collectionMap.end())
			return findResult->second;
		else
			return NULL;
	}

	///<summary>Gets the highest ID used in the specified collection</summary>
	///<param name="collectionName">The name of the collection to check</param>
	///<returns>0 or greater if successful, -1 if collection not found</returns>
	static long long GetHighestIDInCollection(const std::string& collectionName);

	///<summary>Checks if the name exists within the collection</summary>
	///<param name="collectionName">Name of the collection to check for</param>
	///<returns>True if exists, false if not</returns>
	static inline bool CollectionExists(const std::string& collectionName)
	{
		CollectionMap::iterator result = collectionMap.find(collectionName);

		return (result != collectionMap.end());
	}

	///<summary>Copies the elements of one collection to another</summary>
	///<param name="copyToName">The name of the collection to copy to</param>
	///<param name="copyFromName">The name of the collection to copy from</param>
	///<returns>True if successful, false if a name isn't found</returns>
	static bool CopyCollection(const std::string& copyToName, const std::string& copyFromName);

	///<summary>Creates IDs for all objects in the specified collection
	///if an ID exists, it is skipped. Cannot be 0</summary>
	///<param name="collectionName">The name of the collection to create ID's for</param>
	///<param name="idToStartFrom">The ID to start from</param>
	///<returns>True if collection exists, false if not</returns>
	static bool CreateIDs(const std::string& collectionName, physx::PxSerialObjectId idToStartFrom);

	///<summary>Creates IDs for all collections, from specified id</summary>
	///<param name="idToStartFrom">The ID to start from, cannot be 0</param>
	static void CreateIDs(physx::PxSerialObjectId idToStartFrom);

	///<summary>Create IDs for all collections from specified id.</summary>
	///<param name="idsToStartFrom">The vector holding the IDs. 
	///if it doesn't match the size of the collections, it only goes to that point,
	///if its too much it will only go up to the number of collections. None can be 0</param>
	///<returns>The number of collections that had ID's created</returns>
	static unsigned int CreateIDs(const std::vector<physx::PxSerialObjectId>& idsToStartFrom);

	static physx::PxSerialObjectId GetID(const std::string& collectionName, const physx::PxBase& object);

	///<summary>Finds an object in a given collection under the specified id</summary>
	///<param name="collectionName">The name of the collection to search</param>
	///<param name="idToFind">The id to locate within the collection</param>
	///<returns>The object if found, NULL if not</returns>
	static physx::PxBase* FindByID(const std::string& collectionName, const physx::PxSerialObjectId idToFind);

	///<summary>Needs to be called when collection is finished being filled, but just before serializing</summary>
	///<param name="collectionToComplete">The name of the collection to Complete</param>
	///<param name="referenceCollection">The name of the collection that represents serializable objects
	///to be excluded from the Completed Collection</param>
	///<returns>True if successful, false if the Registry wasn't initialized or error occured</returns>
	static bool CompleteCollection(const std::string& collectionToComplete,
		const std::string& referenceCollection = std::string(""));

	static bool SerializeToBinary(const std::string& fileName, const std::string& collectionToSerialize,
		const std::string& externalRefsCollection = std::string(""));

	static bool SerializeToXML(const std::string& fileName, const std::string& collectionToSerialize,
		const std::string& externalRefsCollection = std::string(""));

	///<summary>Deserialize a binary file</summary>
	///<param name="fileName">The name of the file to deserialize</param>
	///<param name="collectionToFill">The name to assign a new collection with</param>
	///<returns>True if successful, false if file error, false if collection already exists</returns>
	static bool DeserializeFromBinary(const std::string& fileName, const std::string& collectionToFill,
		const std::string& externalRefsCollection = std::string(""));

	///<summary>Adds the collection to the specified physX scene</summary>
	///<param name="sceneToAddTo">The scene to add the collection to</param>
	///<param name="collectionToAdd">The name of the collection to add</param>
	///<returns>True if successful, false if collection doesn't exist</returns>
	static bool AddToScene(physx::PxScene* sceneToAddTo, const std::string& collectionToAdd);

	///<summary>Release a specific collection</summary>
	///<param name="collectionName">The name of the collection to release</param>
	///<returns>True if successful, false if collection under specified name isn't found</returns>
	static bool ReleaseCollection(const std::string& collectionName);

	///<summary>Releases the memory of all stored collections</summary>
	static void ReleaseAllCollections();


};

