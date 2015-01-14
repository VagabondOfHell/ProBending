#include "PhysXDataManager.h"
#include "PhysXCookingWrapper.h"
#include "MeshInfo.h"
#include "ShapeDefinition.h"

#include "cooking/PxConvexMeshDesc.h"
#include "PxPhysics.h"
#include "extensions/PxDefaultStreams.h"

using namespace physx;

PhysXDataManager* PhysXDataManager::instance;

PhysXDataManager::PhysXDataManager(void)
{
}

PhysXDataManager::~PhysXDataManager(void)
{
	for (auto start = geometryMap.begin(); start != geometryMap.end(); ++start)
	{
		start->second.reset();
	}

	geometryMap.clear();

	for (auto start = materialMap.begin(); start != materialMap.end(); ++start)
	{
		start->second->release();
	}

	materialMap.clear();
}

physx::PxMaterial* PhysXDataManager::CreateMaterial(const physx::PxReal staticFriction, 
			const physx::PxReal dynamicFriction, const physx::PxReal restitution, 
			const std::string& name /*= std::string("")*/)
{
	MaterialMap::iterator hint = materialMap.begin();

	bool saveMaterial = !name.empty();

	if(saveMaterial)
	{
		if(FindExistingWithHint(name, hint))
			return hint->second;
	}

	physx::PxMaterial* newMat = PxGetPhysics().createMaterial(staticFriction, dynamicFriction, restitution);
	
	printf("Physics Material Count: %i\n", PxGetPhysics().getNbMaterials());

	if(newMat)
		if(saveMaterial)
			materialMap.insert(hint, MaterialMap::value_type(name, newMat));

	return newMat;

}

SharedBoxGeo PhysXDataManager::CreateBoxGeometry(const physx::PxVec3& halfExtents, 
			const std::string& name /*= std::string("")*/)
{
	GeometryMap::iterator hint = geometryMap.begin();//start at beginning so we can use the hint even if we don't search the map

	bool saveShape = !name.empty();//if we are to save the geometry, set it to whether or not name is empty
	
	if(saveShape)//if we are saving, make sure it doesn't exist already
	{
		if(FindExistingWithHint(name, hint))
		{
			// key already exists
			SharedGeo currentGeo = hint->second;

			if(currentGeo->getType() == physx::PxGeometryType::eBOX)
				return std::static_pointer_cast<physx::PxBoxGeometry>(currentGeo);
			else
				return NULL; //Return wrong type 
		}
	}

	SharedBoxGeo returnedGeometry = SharedBoxGeo(new PxBoxGeometry(halfExtents)); //Create new box at this point

	if(saveShape)//insert if saving with hint from above
		geometryMap.insert(hint, GeometryMap::value_type(name, returnedGeometry));
	
	return returnedGeometry; 
}

SharedSphereGeo PhysXDataManager::CreateSphereGeometry(float radius, const std::string& name /*= std::string("")*/)
{
	GeometryMap::iterator hint = geometryMap.begin();//start at beginning so we can use the hint even if we don't search the map

	bool saveShape = !name.empty();//if we are to save the geometry, set it to whether or not name is empty

	if(saveShape)//if we are saving, make sure it doesn't exist already
	{
		if(FindExistingWithHint(name, hint))
		{
			// key already exists
			SharedGeo currentGeo = hint->second;

			if(currentGeo->getType() == physx::PxGeometryType::eBOX)
				return std::static_pointer_cast<physx::PxSphereGeometry>(currentGeo);
			else
				return NULL; //Return wrong type 
		}
	}

	SharedSphereGeo returnedGeometry = SharedSphereGeo(new PxSphereGeometry(radius)); //Create new box at this point

	if(saveShape)//insert if saving with hint from above
		geometryMap.insert(hint, GeometryMap::value_type(name, returnedGeometry));

	return returnedGeometry; 
}

SharedConvexMeshGeo PhysXDataManager::CreateConvexMeshGeometry(std::shared_ptr<MeshInfo> meshInfo,
					const std::string& name /*= std::string("")*/)
{
	GeometryMap::iterator hint = geometryMap.begin();//start at beginning so we can use the hint even if we don't search the map

	bool saveShape = !name.empty();//if we are to save the geometry, set it to whether or not name is empty

	if(saveShape)//if we are saving, make sure it doesn't exist already
	{
		if(FindExistingWithHint(name, hint))
		{
			// key already exists
			SharedGeo currentGeo = hint->second;

			if(currentGeo->getType() == physx::PxGeometryType::eCONVEXMESH)
				return std::static_pointer_cast<physx::PxConvexMeshGeometry>(currentGeo);
			else
				return NULL; //Return wrong type 
		}
	}

	SharedConvexMeshGeo convexMeshShared = NULL;

	if(PhysXCookingWrapper::CookingValid())
	{
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count     = meshInfo->vertex_count;
		convexDesc.points.stride    = sizeof(Ogre::Vector3);
		convexDesc.points.data      = meshInfo->vertices;
		convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxDefaultMemoryOutputStream buf;

		physx::PxConvexMeshCookingResult::Enum result;
	
		if(PhysXCookingWrapper::cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			physx::PxConvexMesh* convexMesh = PxGetPhysics().createConvexMesh(input);
			convexMeshShared = SharedConvexMeshGeo(new physx::PxConvexMeshGeometry(convexMesh));
		}
	}
	
	if(saveShape && convexMeshShared)//insert if saving with hint from above
		geometryMap.insert(hint, GeometryMap::value_type(name, convexMeshShared));

	return convexMeshShared; 
}

physx::PxShape* PhysXDataManager::CreateShape(const ShapeDefinition& shapeDefinition, const std::string& name /*= std::string("")*/)
{
	ShapeMap::iterator hint = shapeMap.begin();//start at beginning so we can use the hint even if we don't search the map

	bool saveShape = !name.empty();//if we are to save the shape, set it to whether or not name is empty

	if(saveShape)//if we are saving, make sure it doesn't exist already
	{
		if(FindExistingWithHint(name, hint))
			// key already exists
			return hint->second;
	}

	if(shapeDefinition.MaterialList.size() == 0)//if we are forced to create it, make sure we have valid structure
		return NULL;

	physx::PxShape* shape = PxGetPhysics().createShape(*shapeDefinition.ShapeGeometry.get(), 
		&shapeDefinition.MaterialList[0], shapeDefinition.MaterialList.size(), 
		!saveShape, shapeDefinition.ShapeFlags);

	if(shape)
	{
		shape->setLocalPose(shapeDefinition.Transform);

		if(saveShape)
			shapeMap.insert(hint, ShapeMap::value_type(name, shape));
	}

	return shape;
}
