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

struct PxDataManSerializeOptions
{
	enum DataSerializers{
		NONE = 0, 
		MATERIALS = 1,  
		CONVEX_MESHES = 1 << 1,
		TRIANGLE_MESHES = 1 << 2,
		HEIGHT_FIELD = 1 << 3,
		SHAPES = 1 << 4,
		ALL = 31
	};

	typedef char SerializationResults;

	DataSerializers SerializerDataTypes;
	std::string CollectionName;
	std::string FileName;//Name of file without extension
	bool PersistantCollection;//True to not release collection when finished

	long long ShapeStartID, MaterialStartID, ConvexMeshStartID, TriMeshStartID, HeightFieldStartID;

	///<summary>Constructor of the customizable serialization options</summary>
	///<param name="dataSerializer">Options on what to serialize</param>
	///<param name="collectionName">Name to assign to the collection to save</param>
	///<param name="persistantCollection">False to release collection after use, true to keep</param>
	///<param name="fileName">The name to assign to the file, without extension</param>
	PxDataManSerializeOptions(DataSerializers dataSerializer = ALL, 
		const std::string& collectionName = "", bool persistantCollection = false,
		const std::string& fileName = "", long long shapeStartID = 0, long long materialStartID = 0, 
		long long convexMeshStartID = 0, long long triMeshStartID = 0, long long heightFieldStartID = 0)
		: SerializerDataTypes(dataSerializer), CollectionName(collectionName), FileName(fileName),
		PersistantCollection(persistantCollection), ShapeStartID(shapeStartID), MaterialStartID(materialStartID),
		ConvexMeshStartID(convexMeshStartID), TriMeshStartID(triMeshStartID), HeightFieldStartID(heightFieldStartID)
	{
	}

	~PxDataManSerializeOptions()
	{
	}
};

class MeshInfo;

class PhysXDataManager
{
private:
	static PhysXDataManager* instance;

	typedef std::map<std::string, physx::PxShape*> ShapeMap;
	typedef std::map<std::string, physx::PxMaterial*> MaterialMap;
	typedef std::map<std::string, physx::PxConvexMesh*> ConvexMeshMap;
	typedef std::map<std::string, physx::PxHeightField*>HeightFieldMap;
	typedef std::map<std::string, physx::PxTriangleMesh*> TriangleMeshMap;
	
	ConvexMeshMap convexMeshMap;
	TriangleMeshMap triangleMeshMap;
	HeightFieldMap heightFieldMap;

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

	inline bool FindExistingWithHint(const std::string& nameToFind, ConvexMeshMap::iterator& outVal)
	{
		outVal = convexMeshMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != convexMeshMap.end() && !(convexMeshMap.key_comp()(nameToFind, outVal->first)));
	}

	inline bool FindExistingWithHint(const std::string& nameToFind, TriangleMeshMap::iterator& outVal)
	{
		outVal = triangleMeshMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != triangleMeshMap.end() && !(triangleMeshMap.key_comp()(nameToFind, outVal->first)));
	}

	inline bool FindExistingWithHint(const std::string& nameToFind, HeightFieldMap::iterator& outVal)
	{
		outVal = heightFieldMap.lower_bound(nameToFind);
		//returns true if existing
		return (outVal != heightFieldMap.end() && !(heightFieldMap.key_comp()(nameToFind, outVal->first)));
	}

	bool SaveXML(const std::string& fileName, const std::string& collectionName);

	bool LoadXML(const std::string& fileName, const std::string& collectionName);

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

	void ReleaseAll();

	///<summary>Creates a new physics material</summary>
	///<param name="staticFriction">The static friction applied when object is not moving,
	///between 0.0f and 1.0f</param>
	///<param name="dynamicFriction">The dynamic friction applied when object is moving, between 0.0f and 1.0f</param>
	///<param name="restitution">The restitution of collision, between 0.0f and 1.0f</param>
	///<param name="name">The name to store the material under, or leave blank to not save</param>
	///<returns>Pointer to the created material</returns>
	physx::PxMaterial* CreateMaterial(const physx::PxReal staticFriction,
		const physx::PxReal dynamicFriction, const physx::PxReal restitution, 
		const std::string& name);

	///<summary>Creates a Convex Mesh based on the passed information</summary>
	///<param name="meshInfo">The info structure detailing the mesh's composition</param>
	///<param name="name">The name to store the mesh geometry under. Use "" to not store</param>
	///<returns>Pointer to the newly created mesh (to save searching it later), or NULL if already created
	///and/or meshInfo is invalid</returns>
	physx::PxConvexMesh* const CookConvexMesh(std::shared_ptr<MeshInfo> meshInfo, 
		const std::string& meshName);

	physx::PxTriangleMesh* CookTriangleMesh(std::shared_ptr<MeshInfo> meshInfo,
		std::string& name);

	physx::PxHeightField* CookHeightField(std::shared_ptr<MeshInfo> meshInfo,
		std::string& name);

	physx::PxShape* CreateShape(const ShapeDefinition& shapeDefinition, 
		const std::string& name);

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
	
	///<summary>Gets the geometry of the specified name as a Convex Mesh Geometry</summary>
	///<param name="name">The unique name representing the shape</param>
	///<returns>Shared pointer to the geometry, or NULL if not found or wrong type</returns>
	inline physx::PxConvexMesh* const GetConvexMesh(const std::string& name)
	{
		ConvexMeshMap::iterator result = convexMeshMap.find(name);

		if(result != convexMeshMap.end())
				return result->second;//if found, return

		return NULL;
	}

	///<summary>Creates a Convex Mesh Geometry off the mesh found with the specified name</summary>
	///<param name="meshName">The name of the mesh to find</param>
	///<returns>Shared pointer to the newly created mesh geometry, or NULL if the mesh name
	///was not found to reference stored mesh</returns>
	inline SharedConvexMeshGeo GetConvexMeshGeometry(const std::string& meshName)
	{
		physx::PxConvexMesh* mesh = GetConvexMesh(meshName);

		if(mesh)
			return SharedConvexMeshGeo(new physx::PxConvexMeshGeometry(mesh));

		return NULL;
	}

	inline size_t GetMaterialCount()const{return materialMap.size();}

	inline size_t GetShapeCount()const{return shapeMap.size();}

	inline size_t GetConvexMeshCount()const{return convexMeshMap.size();}

	inline size_t GetTriangleMeshCount()const{return triangleMeshMap.size();}

	inline size_t GetHeightFieldCount()const{return heightFieldMap.size();}

	///<summary>Gathers the materials stored in this manager and places them in the specified collection</summary>
	///<param name="collectionName">The name of the collection to retrieve or create</param>
	void GatherMaterials(const std::string& collectionName);

	///<summary>Gathers the convex meshes stored in this manager and places them in the specified collection</summary>
	///<param name="collectionName">The name of the collection to retrieve or create</param>
	void GatherConvexMeshes(const std::string& collectionName);

	///<summary>Gathers the triangle meshes stored in this manager and places them in the specified collection</summary>
	///<param name="collectionName">The name of the collection to retrieve or create</param>
	void GatherTriangleMeshes(const std::string& collectionName);

	///<summary>Gathers the height fields stored in this manager and places them in the specified collection</summary>
	///<param name="collectionName">The name of the collection to retrieve or create</param>
	void GatherHeightFields(const std::string& collectionName);

	///<summary>Gathers the materials stored in this manager and places them in the specified collection</summary>
	///<param name="collectionName">The name of the collection to retrieve or create</param>
	///<param name="exceptForCollectionName">Collection of items that will complete the shapes</param>
	///<param name="includeMaterials">True to include materials required by the shape</param>
	///<param name="includeConvexMesh">True to include convex meshes required by the shape</param>
	///<param name="includeTriangleMesh">True to include the triangle meshes required by the shape</param>
	///<param name="includeHeightField">True to include the height field required by the shape</param>
	void GatherShapes(const std::string& collectionName, const std::string& 
		exceptForCollectionName = std::string(""), bool includeMaterials = false,
		bool includeConvexMesh = false, bool includeTriangleMesh = false, bool includeHeightField = false);
	
	///<summary>Saves the data held by this manager</summary>
	///<param name="serializationOptions">The options for customizing serialization</param>
	///<returns>True if successful, false if not. If shapes in this manager were created
	///with materials or meshes that are not saved in this manager, then serializing shapes will fail</returns>
	bool SerializeData(const PxDataManSerializeOptions& serializationOptions
		= PxDataManSerializeOptions());

	bool DeserializeData(const PxDataManSerializeOptions& serializationOptions
		= PxDataManSerializeOptions());
};

