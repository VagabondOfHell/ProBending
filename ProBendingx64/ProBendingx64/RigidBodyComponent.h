#pragma once
#include "Component.h"

#include "foundation/PxSimpleTypes.h"
#include "foundation/PxTransform.h"

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
	class PxShape;
	class PxGeometry;
	class PxMaterial;
};

class RigidBodyComponent :
	public Component
{
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
	Ogre::SceneNode* physxDebugNode; //Rendering Debug outlines of physx shapes
	Ogre::ManualObject* physxDebugDraw; //Mesh representing the shape outline
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

	///<summary>Attaches a previously created shape to the rigid actor</summary>
	///<param name="newShape">The new shape to add</param>
	///<returns>True if successful, false if not. False if no rigid body has been created yet</returns>
	bool AttachShape(physx::PxShape& newShape);

	///<summary>Creates and attaches a shape</summary>
	///<param name="geometry">The geometry representing the shape</param>
	///<param name="material">The material of the shape</param>
	///<param name="transform">The local pose of the shape</param>
	///<returns>True if successful, false if not</returns>
	bool AttachShape(physx::PxGeometry& geometry, physx::PxMaterial& material, physx::PxTransform transform = physx::PxTransform::createIdentity());

	///<summary>Creates and attaches a shape</summary>
	///<param name="geometry">The geometry representing the shape</param>
	///<param name="staticFriction">Value between 0.0 and 1.0</param>
	///<param name="dynamicFriction">Value between 0.0 and 1.0</param>
	///<param name="restitution">Value between 0.0 and 1.0</param>
	///<param name="position">The local position of the shape</param>
	///<param name="orientation">The local orientation of the shape</param>
	///<returns>True if successful, false if not</returns>
	bool AttachShape(physx::PxGeometry& geometry, physx::PxReal staticFriction = 0.0f, 
		physx::PxReal dynamicFriction = 0.0f, physx::PxReal restitution = 0.0f,
		physx::PxVec3& position = physx::PxVec3(0.0f), physx::PxQuat& orientation = physx::PxQuat::createIdentity());

	///<summary>Sets whether the component is affected by gravity or not</summary>
	///<param name="val">True to be affected by gravity, false if not</param>
	void SetUseGravity(const bool val);

	void ApplyImpulse(physx::PxVec3& impulse);

	void ApplyForce(physx::PxVec3& force);

	///<summary>Gets the type of body that this rigid body is</summary>
	///<returns>static, dynamic, or NONE if not yet initialized</returns>
	const RigidBodyType GetBodyType()const{return bodyType;}

	///<summary>Gets the rigid body as a Static Actor. Does not perform any checks</summary>
	///<returns>Pointer to the dynamic body</returns>
	inline physx::PxRigidStatic* const GetStaticActor()const{return bodyStorage.staticActor;}

	///<summary>Gets the rigid body as a Dynamic Actor. Does not perform any checks</summary>
	///<returns>Pointer to the dynamic body</returns>
	inline physx::PxRigidDynamic* const GetDynamicActor()const{return bodyStorage.dynamicActor;}

#if _DEBUG
	void CreateDebugDraw();
#endif

	virtual void Start();

	virtual void Update(float gameTime);

	///<summary>Gets the component type of this component</summary>
	///<returns>The type of component</returns>
	virtual inline ComponentType GetComponentType()
	{
		return Component::RIGID_BODY_COMPONENT;
	}

	///<summary>Gets the type of rigid body of this component</summary>
	///<returns>Enum declaration of rigid body type</returns>
	inline RigidBodyType GetRigidBodyType()
	{
		return bodyType;
	}


};

