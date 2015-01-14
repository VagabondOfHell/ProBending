#pragma once
#include "foundation/PxSimpleTypes.h"
#include "geometry/PxBoxGeometry.h"
#include "geometry/PxSphereGeometry.h"
#include "geometry/PxConvexMeshGeometry.h"
#include "geometry/PxTriangleMeshGeometry.h"
#include "PxMaterial.h"

#include "ShapeDefinition.h"

#include <map>
#include <memory>
#include <string>

class MeshInfo;

class PhysXDataManager
{
private:
	static PhysXDataManager* instance;

	typedef std::map<std::string, physx::PxMaterial*> MaterialMap;
	typedef std::map<std::string, physx::PxShape*> ShapeMap;
	typedef std::map<std::string, SharedGeo> GeometryMap;

	GeometryMap geometryMap;
	MaterialMap materialMap;
	ShapeMap shapeMap;

	PhysXDataManager(void);

	//template<class anyMap>
	//inline bool FindExistingWithHint(const anyMap& map, const std::string& nameToFind, 
	//	typedef typename anyMap::iterator& outVal)
	//{
	//	outVal = map.lower_bound(nameToFind);
	//	//returns true if existing
	//	return (outVal != map.end() && !(map.key_comp()(nameToFind, outVal->first)));
	//}

	inline bool FindExistingWithHint(const std::string& nameToFind, GeometryMap::iterator& outVal)
	{
		outVal = geometryMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != geometryMap.end() && !(geometryMap.key_comp()(nameToFind, outVal->first)));
	}

	inline bool FindExistingWithHint(const std::string& nameToFind, MaterialMap::iterator& outVal)
	{
		outVal = materialMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != materialMap.end() && !(materialMap.key_comp()(nameToFind, outVal->first)));
	}

	inline bool FindExistingWithHint(const std::string& nameToFind, ShapeMap::iterator& outVal)
	{
		outVal = shapeMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != shapeMap.end() && !(shapeMap.key_comp()(nameToFind, outVal->first)));
	}

public:
	
	~PhysXDataManager(void);

	static inline PhysXDataManager* const GetSingletonPtr()
	{
		if(!instance)
			instance = new PhysXDataManager();

		return instance;
	}
	
	static inline void DestroySingleton()
	{
		if(instance)
		{
			delete instance;
			instance = NULL;
		}
	}

	///<summary>Creates a new physics material</summary>
	///<param name="staticFriction">The static friction applied when object is not moving,
	///between 0.0f and 1.0f</param>
	///<param name="dynamicFriction">The dynamic friction applied when object is moving, between 0.0f and 1.0f</param>
	///<param name="restitution">The restitution of collision, between 0.0f and 1.0f</param>
	///<param name="name">The name to store the material under, or leave blank to not save</param>
	///<returns>Pointer to the created material</returns>
	physx::PxMaterial* CreateMaterial(const physx::PxReal staticFriction,
		const physx::PxReal dynamicFriction, const physx::PxReal restitution, 
		const std::string& name = std::string(""));

	///<summary>Creates a PhysX Box geometry of the specified size</summary>
	///<param name="halfExtents">The half-size of the box</param>
	///<param name="name">The name to store the geometry under, or "" to not store</param>
	///<returns>A shared pointer to the required geometry</returns>
	SharedBoxGeo CreateBoxGeometry(const physx::PxVec3& halfExtents, const std::string& name = std::string(""));
	
	///<summary>Creates a PhysX Sphere geometry of the specified size</summary>
	///<param name="radius">The radius of the sphere</param>
	///<param name="name">The name to store the geometry under, or "" to not store</param>
	///<returns>A shared pointer to the required geometry</returns>
	SharedSphereGeo CreateSphereGeometry(const float radius,const std::string& name = std::string(""));

	///<summary>Creates a Convex Mesh geometry based on the passed information</summary>
	///<param name="meshInfo">The info structure detailing the mesh's composition</param>
	///<param name="name">The name to store the mesh geometry under. Use "" to not store</param>
	///<returns>Shared pointer to the created or existing mesh geometry, or NULL if not found and/or 
	///meshInfo is invalid</returns>
	SharedConvexMeshGeo CreateConvexMeshGeometry(std::shared_ptr<MeshInfo> meshInfo, 
		const std::string& name = std::string(""));

	physx::PxShape* CreateShape(const ShapeDefinition& shapeDefinition, 
		const std::string& name = std::string(""));

	///<summary>Tries to get the material of the specified name</summary>
	///<param name="name">The unique name representing the material</param>
	///<returns>Pointer to the material or NULL if not found</returns>
	inline physx::PxMaterial* GetMaterial(const std::string& name)
	{
		MaterialMap::iterator result = materialMap.find(name);

		if(result != materialMap.end())
			return result->second;

		return NULL;
	}
	
	///<summary>Gets the geometry stored under the specified name</summary>
	///<param name="name">The name to search for</param>
	///<returns>The shared pointer of the Geometry, or NULL if not found</returns>
	inline SharedGeo GetGeometry(const std::string& name)
	{
		GeometryMap::iterator result = geometryMap.find(name);

		if(result != geometryMap.end())
				return result->second;

		return NULL;
	}

	///<summary>Gets the geometry of the specified name as a Box Geometry</summary>
	///<param name="name">The unique name representing the shape</param>
	///<returns>Shared pointer to the geometry, or NULL if not found or wrong type</returns>
	inline SharedBoxGeo GetBoxGeometry(const std::string& name)
	{
		GeometryMap::iterator result = geometryMap.find(name);

		if(result != geometryMap.end())
			if(result->second->getType() == physx::PxGeometryType::eBOX)
				return std::static_pointer_cast<physx::PxBoxGeometry>(result->second);

		return NULL;
	}

	///<summary>Gets the geometry of the specified name as a Sphere Geometry</summary>
	///<param name="name">The unique name representing the shape</param>
	///<returns>Shared pointer to the geometry, or NULL if not found or wrong type</returns>
	inline SharedSphereGeo GetSphereGeometry(const std::string& name)
	{
		GeometryMap::iterator result = geometryMap.find(name);

		if(result != geometryMap.end())
			if(result->second->getType() == physx::PxGeometryType::eSPHERE)
				return std::static_pointer_cast<physx::PxSphereGeometry>(result->second);

		return NULL;
	}

	///<summary>Gets the geometry of the specified name as a Convex Mesh Geometry</summary>
	///<param name="name">The unique name representing the shape</param>
	///<returns>Shared pointer to the geometry, or NULL if not found or wrong type</returns>
	inline SharedConvexMeshGeo GetConvexMeshGeometry(const std::string& name)
	{
		GeometryMap::iterator result = geometryMap.find(name);

		if(result != geometryMap.end())
			if(result->second->getType() == physx::PxGeometryType::eCONVEXMESH)
				return std::static_pointer_cast<physx::PxConvexMeshGeometry>(result->second);

		return NULL;
	}

};

