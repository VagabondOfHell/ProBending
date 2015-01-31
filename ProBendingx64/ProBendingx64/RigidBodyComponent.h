#pragma once
#include "Component.h"
#include "ShapeDefinition.h"

#include <memory>
#include <map>

#if _DEBUG
namespace Ogre
{
	class SceneNode;
	class ManualObject;
};
#endif

namespace physx
{
	class PxRigidDynamic;
	class PxRigidStatic;
};

class RigidBodyComponent :
	public Component
{
	friend class SceneSerializer;
public:
	enum RigidBodyType{NONE, STATIC, DYNAMIC}; ///The types of possible rigid bodies

protected:

	RigidBodyType bodyType; //The type of rigid body

	union PhysXBodyStorage //Union to store either a static or dynamic actor
	{
		physx::PxRigidStatic* staticActor;
		physx::PxRigidDynamic* dynamicActor;
	};

	PhysXBodyStorage bodyStorage;

#if _DEBUG
	//std::vector<Ogre::SceneNode*> physxDebugNodes; //Rendering Debug outlines of physx shapes
	Ogre::SceneNode* physxDebugNode; //Rendering Debug outlines of physx shapes
#endif

public:
	RigidBodyComponent();
	virtual ~RigidBodyComponent(void);

	///<summary>Creates a PhysX Rigid Body</summary>
	///<param name="bodyType">Select whether static or dynamic</param>
	///<param name="position">The position of the rigid body</param>
	///<param name="orientation">The rotation of the rigid body</param>
	///<returns>True if successful, false if not. False if this has already been called, 
	///invalid body type, or physX was unsuccessful in creating it</returns>
	bool CreateRigidBody(RigidBodyType bodyType, physx::PxVec3& position = physx::PxVec3(0.0f), 
		physx::PxQuat& orientation = physx::PxQuat::createIdentity());

	///<summary>Create and attach a new shape to this rigid body</summary>
	///<param name="shapeDefinition">The geometry representing the shape</param>
	///<returns>True if successful, false if Shape definition is not correctly filled out</returns>
	bool CreateAndAttachNewShape(const ShapeDefinition& shapeDefinition);

	///<summary>Attaches a previously created shape to the rigid actor</summary>
	///<param name="newShape">The new shape to add</param>
	///<returns>True if successful, false if not. False if no rigid body has been created yet</returns>
	bool AttachShape(physx::PxShape& newShape);

	///<summary>Sets whether the component is affected by gravity or not</summary>
	///<param name="val">True to be affected by gravity, false if not</param>
	void SetUseGravity(const bool val);

	///<summary>Applies an impulse force if the body is a dynamic body</summary>
	///<param name="impulse">The amount of force to apply</param>
	void ApplyImpulse(physx::PxVec3& impulse);

	///<summary>Applies a force to the body if it is a dynamic body</summary>
	///<param name="force">The amount of force to apply</param>
	void ApplyForce(physx::PxVec3& force);

	void SetVelocity(physx::PxVec3& newVel);

	void SetPosition(physx::PxVec3& position);

	void SetOrientation(physx::PxQuat& orientation);

	void ClearForces();

	///<summary>Gets the type of body that this rigid body is</summary>
	///<returns>static, dynamic, or NONE if not yet initialized</returns>
	const inline RigidBodyType GetBodyType()const{return bodyType;}

	///<summary>Gets the rigid body as a Static Actor. Does not perform any checks</summary>
	///<returns>Pointer to the dynamic body</returns>
	inline physx::PxRigidStatic* const GetStaticActor()const{return bodyStorage.staticActor;}

	///<summary>Gets the rigid body as a Dynamic Actor. Does not perform any checks</summary>
	///<returns>Pointer to the dynamic body</returns>
	inline physx::PxRigidDynamic* const GetDynamicActor()const{return bodyStorage.dynamicActor;}

#if _DEBUG
	void CreateDebugDraw();

	void PrintRigidData();

#endif

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Gets the component type of this component</summary>
	///<returns>The type of component</returns>
	virtual inline ComponentType GetComponentType()
	{
		return Component::RIGID_BODY_COMPONENT;
	}

	RigidBodyComponent* Clone(GameObject* gameObject);
};

