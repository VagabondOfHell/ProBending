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

	static inline void* CreateAlignedBlock(physx::PxInputData& data, physx::PxU8*& baseAddr)
	{
		physx::PxU32 length = data.getLength();
		baseAddr = (physx::PxU8*)malloc(length+PX_SERIAL_FILE_ALIGN-1);
		void* alignedBlock = (void*)((size_t(baseAddr)+PX_SERIAL_FILE_ALIGN-1)&~(PX_SERIAL_FILE_ALIGN-1));
		data.read(alignedBlock, length);
		return alignedBlock;
	}

public:

	///<summary>Creates the Serializer using the PhysX Extension</summary>
	///<returns>True if successful, false if already created or failure</returns>
	static bool CreateSerializer();

	///<summary>Releases the Serializer extension. Also releases all collections</summary>
	static void DestroySerializer();

	static void CleanMemory()
	{
		for (auto start = dynamicMemoryList.begin(); start != dynamicMemoryList.end(); ++start)
		{
			delete[] *start;
		}

		dynamicMemoryList.clear();
	}

	///<summary>Creates a new collection under the specified name</summary>
	///<param name="collectionName">The name to store the collection under. Must be Unique</param>
	///<returns>True if successful, false if blank name, name exists, or creation fails</returns>
	static inline bool CreateCollection(const std::string& collectionName)
	{
		return CreateAndGetCollection(collectionName) != NULL;
	}

	///<summary>Adds a physX object to the collection to be serialized</summary>
	///<param name="collectionName">The name of the collection to add to</param>
	///<param name="objectToAdd">The object to add to the collection</param>
	///<param name="objectID">The optional ID to store the collection under. 
	///Must be Unique to the collection. Used to connect references</param>
	///<returns>True if successful, false if object exists in collection or ID exists</returns>
	static bool AddToCollection(const std::string& collectionName, physx::PxBase& objectToAdd, 
		physx::PxSerialObjectId objectID = PX_SERIAL_OBJECT_ID_INVALID);

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

	///<summary>Copies the elements of one collection to another</summary>
	///<param name="copyToName">The name of the collection to copy to</param>
	///<param name="copyFromName">The name of the collection to copy from</param>
	///<returns>True if successful, false if a name isn't found</returns>
	static bool CopyCollection(const std::string& copyToName, const std::string& copyFromName);

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
	static bool DeserializeFromBinary(const std::string& fileName, const std::string& collectionToFill);

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

