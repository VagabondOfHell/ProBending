#include "ShapeDefinition.h"
#include "PhysXDataManager.h"

using namespace physx;

ShapeDefinition::ShapeDefinition(const bool _isTrigger /*= false*/, 
								 const physx::PxVec3& position /*= physx::PxVec3(0.0f)*/, 
								 const physx::PxQuat& orientation /*= physx::PxQuat(physx::PxIdentity)*/)
								 : Transform(position, orientation)
{
	FilterFlags = 0;

	SetShapeFlags(false, true, _isTrigger);
}

ShapeDefinition::~ShapeDefinition()
{
}

void ShapeDefinition::SetShapeFlags(const bool particleDrainShape, const bool sceneQueryShape, const bool triggerShape)
{
	if(particleDrainShape)
		ShapeFlags |= PxShapeFlag::ePARTICLE_DRAIN;
	if(sceneQueryShape)
		ShapeFlags |= PxShapeFlag::eSCENE_QUERY_SHAPE;

	SetTriggerFlags(triggerShape);
}

void ShapeDefinition::AddMaterial(physx::PxMaterial* const material)
{
	if(material)
		MaterialList.push_back(material);
}

void ShapeDefinition::AddMaterial(const std::string& name)
{
	physx::PxMaterial* material = PhysXDataManager::GetSingletonPtr()->GetMaterial(name);
	MaterialList.push_back(material);
}

void ShapeDefinition::SetTriggerFlags(const bool isTrigger)
{
	if(isTrigger)//if trigger shape, disable simulation shape and enable trigger
	{
		ShapeFlags.clear(PxShapeFlag::eSIMULATION_SHAPE);
		ShapeFlags |= PxShapeFlag::eTRIGGER_SHAPE;
	}
	else//otherwise do the opposite. Results in shape always being either trigger or simulation
	{
		ShapeFlags.clear(PxShapeFlag::eTRIGGER_SHAPE);
		ShapeFlags |= PxShapeFlag::eSIMULATION_SHAPE;
	}
}

bool ShapeDefinition::SetGeometry(const SharedGeo& geometry)
{
	if(geometry != NULL)
		ShapeGeometry = geometry;

	return geometry != NULL;
}

void ShapeDefinition::SetBoxGeometry(const physx::PxVec3& halfExtents)
{
	ShapeGeometry = std::make_shared<PxBoxGeometry>(halfExtents);
}

void ShapeDefinition::SetBoxGeometry(const physx::PxReal x, const physx::PxReal y, const physx::PxReal z)
{
	ShapeGeometry = SharedGeo(new PxBoxGeometry(x, y, z));
}

void ShapeDefinition::SetSphereGeometry(const physx::PxReal radius)
{
	ShapeGeometry = SharedGeo(new PxSphereGeometry(radius));
}

bool ShapeDefinition::SetConvexMeshGeometry(const std::string& meshName)
{
	//find the mesh
	SharedConvexMeshGeo geo = PhysXDataManager::GetSingletonPtr()->GetConvexMeshGeometry(meshName);

	if(geo.get() != NULL)
		ShapeGeometry = geo;

	return geo.get() != NULL;
}

bool ShapeDefinition::SetConvexMeshGeometry(physx::PxConvexMesh* convexMesh, 
		physx::PxVec3& scale /*= physx::PxVec3(1.0f)*/, physx::PxQuat& rot/* = physx::PxQuat(physx::PxIdentity)*/)
{
	if(convexMesh)
		ShapeGeometry = SharedConvexMeshGeo(new PxConvexMeshGeometry(convexMesh, physx::PxMeshScale(scale, rot)));

	return convexMesh != NULL;
}

void ShapeDefinition::SetPlaneGeometry()
{
	ShapeGeometry = std::make_shared<physx::PxPlaneGeometry>(physx::PxPlaneGeometry());
}

void ShapeDefinition::SetFilterFlags(const unsigned int filterFlags)
{
	FilterFlags = filterFlags;
}
