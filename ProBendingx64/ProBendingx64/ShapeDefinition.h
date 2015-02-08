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
typedef std::shared_ptr<physx::PxHeightFieldGeometry> SharedHeightFieldGeo;
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

	///<summary>Sets the shapes Geometry to a box geometry</summary>
	///<param name="halfExtents">The half size of the box</param>
	void SetBoxGeometry(const physx::PxVec3& halfExtents);

	///<summary>Sets the shapes Geometry to a box geometry</summary>
	///<param name="x">Half width on x axis</param>
	///<param name="y">Half height on y axis</param>
	///<param name="z">Half depth on z axis</param>
	void SetBoxGeometry(const physx::PxReal x, const physx::PxReal y, const physx::PxReal z);

	///<summary>Sets the shapes Geometry to a sphere geometry</summary>
	///<param name="radius">The radius of the sphere</param>
	void SetSphereGeometry(const physx::PxReal radius);

	///<summary>Sets the shapes Geometry to a Convex Mesh geometry</summary>
	///<param name="meshName">The name of the mesh as stored in the PhysXDataManager</param>
	///<returns>True if found and assigned, false if not</returns>
	bool SetConvexMeshGeometry(const std::string& meshName);

	///<summary>Sets the shapes Geometry to a Convex Mesh geometry</summary>
	///<param name="convexMesh">The convex mesh to construct the geometry from</param>
	///<returns>True if successful, false if not</returns>
	bool SetConvexMeshGeometry(physx::PxConvexMesh* convexMesh, 
		physx::PxVec3& scale = physx::PxVec3(1.0f), physx::PxQuat& rot = physx::PxQuat(physx::PxIdentity));

	///<summary>Sets the geometry to be that of a plane. Direction and position are determined by Shapes Pose,
	///not by the geometry</summary>
	void SetPlaneGeometry();

	///<summary>Sets the shapes Geometry</summary>
	///<param name="geometry">The geometry to assign</param>
	///<returns>True if geometry is not NULL, false if not</returns>
	bool SetGeometry(const SharedGeo& geometry);
};

