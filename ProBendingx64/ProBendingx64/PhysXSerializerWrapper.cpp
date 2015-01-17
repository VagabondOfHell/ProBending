#include "PhysXSerializerWrapper.h"
#include "PxPhysics.h"
#include "extensions/PxSerialization.h"
#include "extensions/PxDefaultStreams.h"
#include "PhysXCookingWrapper.h"
#include "PxScene.h"
#include "extensions/PxCollectionExt.h"

PhysXSerializerWrapper::CollectionMap PhysXSerializerWrapper::collectionMap;
std::vector<physx::PxU8*> PhysXSerializerWrapper::dynamicMemoryList;

physx::PxSerializationRegistry* PhysXSerializerWrapper::serializationRegistry = NULL;

using namespace physx;

bool PhysXSerializerWrapper::CreateSerializer()
{
	if(!serializationRegistry)
	{
		serializationRegistry = PxSerialization::createSerializationRegistry(PxGetPhysics());
		return serializationRegistry != NULL;
	}

	return false;
}

void PhysXSerializerWrapper::DestroySerializer()
{
	if(serializationRegistry)
	{
		ReleaseAllCollections();
		serializationRegistry->release();
		serializationRegistry = NULL;
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

bool PhysXSerializerWrapper::DeserializeFromBinary(const std::string& fileName, const std::string& collectionToFill)
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

		collectionFill = PxSerialization::createCollectionFromBinary(alignedBlock, *serializationRegistry);
			
		collectionMap.insert(CollectionMap::value_type(collectionToFill, collectionFill));
		dynamicMemoryList.push_back(baseAddress);

		delete[] dat;

		return collectionFill != NULL;
	}
		
	delete[] dat;
	

	return false;
}

bool PhysXSerializerWrapper::AddToScene(physx::PxScene* sceneToAddTo, const std::string& collectionToAdd)
{
	CollectionMap::iterator findResult = collectionMap.find(collectionToAdd);

	if(findResult != collectionMap.end())
	{
		PxCollection* collection = findResult->second;
		PxCollectionExt::releaseObjects(*collection);
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
	
	collectionMap.clear();
}
