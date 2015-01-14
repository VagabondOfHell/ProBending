#pragma once
#include "foundation/PxSimpleTypes.h"
#include "foundation/PxTransform.h"
#include "PxShape.h"
#include "geometry/PxBoxGeometry.h"
#include "geometry/PxSphereGeometry.h"
#include "geometry/PxConvexMeshGeometry.h"
#include "geometry/PxTriangleMeshGeometry.h"

#include <memory>
#include <vector>

namespace physx
{
	class PxShape;
	class PxGeometry;
	class PxMaterial;
};

typedef std::shared_ptr<physx::PxGeometry> SharedGeo;
typedef std::shared_ptr<physx::PxBoxGeometry> SharedBoxGeo;
typedef std::shared_ptr<physx::PxSphereGeometry> SharedSphereGeo;
typedef std::shared_ptr<physx::PxConvexMeshGeometry> SharedConvexMeshGeo;
typedef std::shared_ptr<physx::PxTriangleMeshGeometry> SharedTriangleMeshGeo;

class ShapeDefinition
{
	friend class RigidBodyComponent;
	friend class PhysXDataManager;

private:
	std::vector<physx::PxMaterial*> MaterialList;

	///<summary>Used to properly set and unset whether the shape is a Trigger Shape</summary>
	///<param name="isTrigger">True if trigger shape, false if not</param>
	void SetTriggerFlags(const bool isTrigger);

public:
	physx::PxTransform Transform;
	physx::PxShapeFlags ShapeFlags;
	SharedGeo ShapeGeometry;

	ShapeDefinition(const bool _isTrigger = false, 
		const physx::PxVec3& position = physx::PxVec3(0.0f),
		const physx::PxQuat& orientation = physx::PxQuat(physx::PxIdentity));

	~ShapeDefinition();

	void SetShapeFlags(const bool particleDrainShape, const bool sceneQueryShape, const bool triggerShape);

	///<summary>Adds a new material to the list of materials</summary>
	///<param name="material">The material to add. If NULL, this method does nothing</param>
	void AddMaterial(physx::PxMaterial* const material);

	///<summary>Adds an existing material to the list of materials</summary>
	///<param name="name">The name to search for. If no instance exists, it is not added</param>
	void AddMaterial(const std::string& name);

	///<summary>Creates a new material and adds it to the list of materials</summary>
	///<param name="staticFriction">The static friction between 0.0f and 1.0f</param>
	///<param name="dynamicFriction">The dynamic friction between 0.0f and 1.0f</param>
	///<param name="restitution">The 'bounciness' of collision between 0.0f and 1.0f</param>
	///<param name="name">The name to store the material under, or "" to not store</param>
	void AddMaterial(const physx::PxReal staticFriction,
		const physx::PxReal dynamicFriction, const physx::PxReal restitution, 
		std::string& name = std::string(""));

	///<summary>Sets the shapes Geometry</summary>
	///<param name="name">The name to search for</param>
	///<returns>True if geometry exists under the specified name, false if not</returns>
	bool SetGeometry(const std::string& name);

	///<summary>Sets the shapes Geometry</summary>
	///<param name="geometry">The geometry to assign</param>
	///<returns>True if geometry is not NULL, false if not</returns>
	bool SetGeometry(const SharedGeo& geometry);
};

