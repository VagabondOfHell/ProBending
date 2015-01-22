#include "PhysXSerializerWrapper.h"
#include "PxPhysics.h"
#include "extensions/PxSerialization.h"
#include "extensions/PxDefaultStreams.h"
#include "PhysXCookingWrapper.h"
#include "PxScene.h"
#include "extensions/PxCollectionExt.h"

unsigned int PhysXSerializerWrapper::ReferenceCount = 0;

physx::PxCollection* PhysXSerializerWrapper::workingCollection = NULL;

PhysXSerializerWrapper::CollectionMap PhysXSerializerWrapper::collectionMap;
std::vector<physx::PxU8*> PhysXSerializerWrapper::dynamicMemoryList;

physx::PxSerializationRegistry* PhysXSerializerWrapper::serializationRegistry = NULL;

using namespace physx;

bool PhysXSerializerWrapper::CreateSerializer()
{
	if(!serializationRegistry)
	{
		serializationRegistry = PxSerialization::createSerializationRegistry(PxGetPhysics());
		ReferenceCount = 1;
		return serializationRegistry != NULL;
	}

	ReferenceCount++;
	return true;
}

void PhysXSerializerWrapper::DestroySerializer()
{
	if(serializationRegistry)
	{
		if(ReferenceCount == 1)
		{
			ReleaseAllCollections();
			serializationRegistry->release();
			serializationRegistry = NULL;
		}
		
		ReferenceCount--;
	}
}

physx::PxCollection* PhysXSerializerWrapper::CreateAndGetCollection(const std::string& collectionName)
{
	if(collectionName.empty())//Invalid name
		return NULL;

	CollectionMap::iterator hint = collectionMap.begin();

	if(FindExistingWithHint(collectionName, hint ))//if collection exists with that name
		return NULL;//indicate insertion failure

	//Doesn't exist at this point, so create
	physx::PxCollection* newCollection = PxCreateCollection();

	if(newCollection)//if valid
	{
		//insert and return true
		collectionMap.insert(hint, CollectionMap::value_type(collectionName, newCollection));
		return newCollection;
	}

	return NULL;
}

bool PhysXSerializerWrapper::SetWorkingCollection(const std::string& collectionName)
{
	CollectionMap::iterator findResult = collectionMap.find(collectionName);

	if(findResult != collectionMap.end())
	{
		workingCollection = findResult->second;
		return true;
	}

	return false;
}

bool PhysXSerializerWrapper::AddToCollection(const std::string& collectionName, 
		physx::PxBase& objectToAdd, physx::PxSerialObjectId objectID /*= PX_SERIAL_OBJECT_ID_INVALID*/)
{
	CollectionMap::iterator findResult = collectionMap.find(collectionName);
	
	if(findResult != collectionMap.end())
	{
		findResult->second->add(objectToAdd, objectID);
		return true;
	}

	return false;
}

bool PhysXSerializerWrapper::AddToWorkingCollection(physx::PxBase& objectToAdd, 
		physx::PxSerialObjectId objectID /*= PX_SERIAL_OBJECT_ID_INVALID*/)
{
	if(workingCollection)
	{
		workingCollection->add(objectToAdd, objectID);
		return true;
	}
	
	return false;
}

bool PhysXSerializerWrapper::CopyCollection(const std::string& copyToName, 
		const std::string& copyFromName)
{
	CollectionMap::iterator copyToResult = collectionMap.find(copyToName);
	CollectionMap::iterator copyFromResult = collectionMap.find(copyFromName);

	if(copyToResult != collectionMap.end() &&
		copyFromResult != collectionMap.end())
	{
		copyToResult->second->add(*copyFromResult->second);
		return true;
	}

	return false;
}

bool PhysXSerializerWrapper::CompleteCollection(const std::string& collectionToComplete, 
		const std::string& referenceCollection /*= std::string("")*/)
{
	if(!serializationRegistry)
		return false;

	CollectionMap::iterator completeCollection = collectionMap.find(collectionToComplete);

	if(completeCollection != collectionMap.end())
	{
		CollectionMap::iterator refCollectionIter = collectionMap.find(referenceCollection);
		PxCollection* refCollection = NULL;

		if(refCollectionIter != collectionMap.end())
			refCollection = refCollectionIter->second;

		PxSerialization::complete(*completeCollection->second, *serializationRegistry, refCollection);

		return true;
	}

	return false;
}

bool PhysXSerializerWrapper::CreateIDs(const std::string& collectionName, physx::PxSerialObjectId idToStartFrom)
{
	CollectionMap::iterator completeCollection = collectionMap.find(collectionName);

	if(completeCollection != collectionMap.end())
	{
		PxSerialization::createSerialObjectIds(*completeCollection->second, idToStartFrom);
		return true;
	}

	return false;
}

void PhysXSerializerWrapper::CreateIDs(physx::PxSerialObjectId idToStartFrom)
{
	for (CollectionMap::iterator start = collectionMap.begin(); start != collectionMap.end(); ++start)
	{
		PxSerialization::createSerialObjectIds(*start->second, idToStartFrom);
	}
}

unsigned int PhysXSerializerWrapper::CreateIDs(const std::vector<physx::PxSerialObjectId>& idsToStartFrom)
{
	std::vector<physx::PxSerialObjectId>::const_iterator vectorStart = idsToStartFrom.begin();

	unsigned int numberOfIDsAdded = 0;

	//Loop through the vector and the map, and go to the shortest one
	for (CollectionMap::iterator start = collectionMap.begin(); 
		start != collectionMap.end() && vectorStart != idsToStartFrom.end(); 
		++start, ++vectorStart, ++numberOfIDsAdded)
	{
		PxSerialization::createSerialObjectIds(*start->second, *vectorStart);
	}

	return numberOfIDsAdded;
}

physx::PxSerialObjectId PhysXSerializerWrapper::GetID
	(const std::string& collectionName, const physx::PxBase& object)
{
	CollectionMap::iterator result = collectionMap.find(collectionName);

	if(result != collectionMap.end())
	{
		return result->second->getId(object);
	}
	
	return PX_SERIAL_OBJECT_ID_INVALID;
}

physx::PxBase* PhysXSerializerWrapper::FindByID(const std::string& collectionName, 
		const physx::PxSerialObjectId idToFind)
{
	CollectionMap::iterator result = collectionMap.find(collectionName);

	if(result != collectionMap.end())
	{
		return result->second->find(idToFind);
	}

	return NULL;
}

bool PhysXSerializerWrapper::SerializeToBinary(const std::string& fileName, 
		const std::string& collectionToSerialize, const std::string& externalRefsCollection /*= std::string("")*/)
{
	CollectionMap::iterator result = collectionMap.find(collectionToSerialize);

	if(result != collectionMap.end())
	{
		physx::PxCollection* refCollection = NULL;

		CollectionMap::iterator refsResult = collectionMap.find(externalRefsCollection);

		if(refsResult != collectionMap.end())
			refCollection = refsResult->second;

		PxDefaultFileOutputStream outputStream = PxDefaultFileOutputStream(fileName.c_str());

		if(outputStream.isValid())
			return PxSerialization::serializeCollectionToBinary(outputStream, *result->second, 
				*serializationRegistry, refCollection, false);
	}

	return false;
}

bool PhysXSerializerWrapper::SerializeToXML(const std::string& fileName, 
		const std::string& collectionToSerialize, const std::string& externalRefsCollection /*= std::string("")*/)
{
	CollectionMap::iterator result = collectionMap.find(collectionToSerialize);

	if(result != collectionMap.end())
	{
		physx::PxCollection* refCollection = NULL;

		CollectionMap::iterator refsResult = collectionMap.find(externalRefsCollection);

		if(refsResult != collectionMap.end())
			refCollection = refsResult->second;

		PxDefaultFileOutputStream outputStream = PxDefaultFileOutputStream(fileName.c_str());

		try
		{
			return PxSerialization::serializeCollectionToXml(outputStream, *result->second,
				*serializationRegistry, PhysXCookingWrapper::cooking, refsResult->second, NULL);
		}
		catch(std::exception e)
		{
			return false;
		}
		
	}

	return false;
}

bool PhysXSerializerWrapper::DeserializeFromBinary(const std::string& fileName, 
		const std::string& collectionToFill, const std::string& externalRefsCollection /*= std::string("")*/)
{
	PxDefaultFileInputData openFile(fileName.c_str());

	if(!openFile.isValid())
		return false;

	physx::PxCollection* collectionFill = NULL;
	CollectionMap::iterator findResult;

	if(FindExistingWithHint(collectionToFill, findResult))
		return false;
			
	physx::PxU32 len = openFile.getLength();
	PxU8* dat = new PxU8[len];

	physx::PxU16 result = openFile.read(dat, openFile.getLength());

	if(result)
	{
		physx::PxDefaultMemoryInputData inputDat(dat, len);
			
		PxU8* baseAddress;

		void* alignedBlock = NULL;
		alignedBlock = CreateAlignedBlock(inputDat, baseAddress);

		CollectionMap::iterator refFindResult = collectionMap.find(externalRefsCollection);
		physx::PxCollection* externalRef = NULL;

		if(refFindResult != collectionMap.end())
			externalRef = refFindResult->second;

		collectionFill = PxSerialization::createCollectionFromBinary(alignedBlock, 
			*serializationRegistry, externalRef);
			
		collectionMap.insert(CollectionMap::value_type(collectionToFill, collectionFill));
		dynamicMemoryList.push_back(baseAddress);
	}
	
	delete[] dat;
	return collectionFill != NULL;
}

bool PhysXSerializerWrapper::AddToScene(physx::PxScene* sceneToAddTo, const std::string& collectionToAdd)
{
	CollectionMap::iterator findResult = collectionMap.find(collectionToAdd);

	if(findResult != collectionMap.end())
	{
		PxCollection* collection = findResult->second;
		
		sceneToAddTo->addCollection(*findResult->second);
		
		return true;
	}
	
	return false;
}

bool PhysXSerializerWrapper::ReleaseCollection(const std::string& collectionName)
{
	CollectionMap::iterator findResult = collectionMap.find(collectionName);

	if(findResult != collectionMap.end())
	{
		findResult->second->release();
		
		if(findResult->second == workingCollection)
			ClearWorkingCollection();
		
		collectionMap.erase(findResult);

		return true;
	}

	return false;
}

void PhysXSerializerWrapper::ReleaseAllCollections()
{
	for (CollectionMap::iterator start = collectionMap.begin(); 
		start != collectionMap.end(); ++start)
	{
		start->second->release();
	}
	
	ClearWorkingCollection();
	collectionMap.clear();
}

