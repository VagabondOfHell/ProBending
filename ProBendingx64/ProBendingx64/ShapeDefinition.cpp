#include "ShapeDefinition.h"
#include "PhysXDataManager.h"

using namespace physx;

ShapeDefinition::ShapeDefinition(const bool _isTrigger /*= false*/, 
								 const physx::PxVec3& position /*= physx::PxVec3(0.0f)*/, 
								 const physx::PxQuat& orientation /*= physx::PxQuat(physx::PxIdentity)*/)
								 : Transform(position, orientation)
{
	SetTriggerFlags(_isTrigger);
}

ShapeDefinition::~ShapeDefinition()
{
}

void ShapeDefinition::SetShapeFlags(const bool particleDrainShape, const bool sceneQueryShape, const bool triggerShape)
{
	physx::PxShapeFlags flags = physx::PxShapeFlags();

	if(particleDrainShape)
		flags.set(physx::PxShapeFlag::ePARTICLE_DRAIN);
	if(sceneQueryShape)
		flags.set(physx::PxShapeFlag::eSCENE_QUERY_SHAPE);

	SetTriggerFlags(triggerShape);
}

void ShapeDefinition::AddMaterial(physx::PxMaterial* const material)
{
	if(material)
		MaterialList.push_back(material);
}

void ShapeDefinition::AddMaterial(const physx::PxReal staticFriction, 
								  const physx::PxReal dynamicFriction, const physx::PxReal restitution, 
								  std::string& name /*= std::string("")*/)
{
	MaterialList.push_back(PhysXDataManager::GetSingletonPtr()->
		CreateMaterial(staticFriction, dynamicFriction, restitution, name));
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
		ShapeFlags.set(PxShapeFlag::eTRIGGER_SHAPE);
	}
	else//otherwise do the opposite. Results in shape always being either trigger or simulation
	{
		ShapeFlags.clear(PxShapeFlag::eTRIGGER_SHAPE);
		ShapeFlags.set(PxShapeFlag::eSIMULATION_SHAPE);
	}
}

bool ShapeDefinition::SetGeometry(const std::string& name)
{
	SharedGeo geo = PhysXDataManager::GetSingletonPtr()->GetGeometry(name);

	if(geo)
		ShapeGeometry = geo;

	return geo != NULL;
}

bool ShapeDefinition::SetGeometry(const SharedGeo& geometry)
{
	if(geometry != NULL)
		ShapeGeometry = geometry;

	return geometry != NULL;
}