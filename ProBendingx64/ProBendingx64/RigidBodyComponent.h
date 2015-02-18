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

	physx::PxVec3 forces;
	physx::PxVec3 velocity;

	inline physx::PxVec3 CalculateAcceleration(){return forces / GetMass();}

	inline physx::PxVec3 CalculateVelocity(const float dt, const physx::PxVec3& accel){return velocity + accel * dt;}

	inline physx::PxVec3 CalculateDisplacement(const float dt, const physx::PxVec3& accel){return (velocity * dt) + 0.5f * accel * (dt * dt);}

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
	bool CreateAndAttachNewShape(const ShapeDefinition& shapeDefinition, const std::string& nameOfShape);

	///<summary>Updates mass and inertia of the body</summary>
	///<param name="bodyMass">The mass of the entire body</param>
	///<param name="centerOfMass">Location of the center of mass</param>
	///<returns>True if successful, false if not. Returns false if Rigid is not a Dynamic body</returns>
	bool CalculateCenterOfMass(const float bodyMass, const physx::PxVec3 centerOfMass = physx::PxVec3(0.0f));

	///<summary>Attaches a previously created shape to the rigid actor</summary>
	///<param name="newShape">The new shape to add</param>
	///<returns>True if successful, false if not. False if no rigid body has been created yet</returns>
	bool AttachShape(physx::PxShape& newShape);

	///<summary>Attaches a shape stored in the PhysX Data Manager under the specified name</summary>
	///<param name="shapeName">Name of the shape to attach </param>
	///<returns>True if exists and attached, false if not found</returns>
	bool AttachShape(const std::string& shapeName);

	///<summary>Sets whether the component is affected by gravity or not</summary>
	///<param name="val">True to be affected by gravity, false if not</param>
	void SetUseGravity(const bool val);

	void ClearForces();

	///<summary>Clears all forces of the kinematic body. If the body isn't Kinematic, call ClearForces instead</summary>
	inline void ClearKinematicForces(){forces = physx::PxVec3(0.0f);}

	inline void ApplyKinematicForce(const physx::PxVec3& force){forces += force;}

	inline void SetKinematicForce(const physx::PxVec3& force){forces = force;}

	inline void SetKinematicVelocity(const physx::PxVec3& vel){velocity = vel;}
	
	///<summary>Applies an impulse force if the body is a dynamic body</summary>
	///<param name="impulse">The amount of force to apply</param>
	void ApplyImpulse(physx::PxVec3& impulse);

	///<summary>Applies a force to the body if it is a dynamic body</summary>
	///<param name="force">The amount of force to apply</param>
	void ApplyForce(physx::PxVec3& force);

	void ApplyTorqueForce(physx::PxVec3& force);

	void ApplyTorqueImpulse(physx::PxVec3& impulse);

	bool IsKinematic();

	float GetMass()const;

	inline void FreezeXRotation()
	{
		if(bodyType == DYNAMIC)
		{
			physx::PxVec3 currVal = GetMassSpaceInertiaTensor();
			currVal.x = 0.0f;
			SetMassSpaceInertiaTensor(currVal);
		}
	}
	
	inline void FreezeYRotation()
	{
		if(bodyType == DYNAMIC)
		{
			physx::PxVec3 currVal = GetMassSpaceInertiaTensor();
			currVal.y = 0.0f;
			SetMassSpaceInertiaTensor(currVal);
		}
	}

	inline void FreezeZRotation()
	{
		if(bodyType == DYNAMIC)
		{
			physx::PxVec3 currVal = GetMassSpaceInertiaTensor();
			currVal.z = 0.0f;
			SetMassSpaceInertiaTensor(currVal);
		}
	}

	inline void FreezeAllRotation()	{SetMassSpaceInertiaTensor(physx::PxVec3(0.0f));}

	inline void UnfreezeXRotation()
	{
		if(bodyType == DYNAMIC)
		{
			physx::PxVec3 currVal = GetMassSpaceInertiaTensor();
			currVal.x = 1.0f;
			SetMassSpaceInertiaTensor(currVal);
		}
	}

	inline void UnfreezeYRotation()
	{
		if(bodyType == DYNAMIC)
		{
			physx::PxVec3 currVal = GetMassSpaceInertiaTensor();
			currVal.y = 1.0f;
			SetMassSpaceInertiaTensor(currVal);
		}
	}

	inline void UnfreezeZRotation()
	{
		if(bodyType == DYNAMIC)
		{
			physx::PxVec3 currVal = GetMassSpaceInertiaTensor();
			currVal.z = 1.0f;
			SetMassSpaceInertiaTensor(currVal);
		}
	}
	
	inline void UnfreezeAllRotation(){SetMassSpaceInertiaTensor(physx::PxVec3(1.0f, 1.0f, 1.0f));}

	physx::PxVec3 GetMassSpaceInertiaTensor()const;

	///<summary>See physX documentation</summary>
	///<param name="diagonal">See PhysX documentation</param>
	void SetMassSpaceInertiaTensor(const physx::PxVec3& diagonal);

	void SetMass(const float newMass);

	void SetKinematic(const bool kinematicOn);

	void SetVelocity(physx::PxVec3& newVel);

	void SetPosition(physx::PxVec3& position);

	void SetOrientation(physx::PxQuat& orientation);

	physx::PxVec3 GetVelocity()const;

	physx::PxVec3 GetPosition()const;

	physx::PxQuat GetOrientation()const;

	///<summary>Gets the type of body that this rigid body is</summary>
	///<returns>static, dynamic, or NONE if not yet initialized</returns>
	const inline RigidBodyType GetBodyType()const{return bodyType;}

	///<summary>Gets the rigid body as a Static Actor. Does not perform any checks</summary>
	///<returns>Pointer to the dynamic body</returns>
	inline physx::PxRigidStatic* const GetStaticActor()const{return bodyStorage.staticActor;}

	///<summary>Gets the rigid body as a Dynamic Actor. Does not perform any checks</summary>
	///<returns>Pointer to the dynamic body</returns>
	inline physx::PxRigidDynamic* const GetDynamicActor()const{return bodyStorage.dynamicActor;}

	void SetKinematicTarget(const physx::PxTransform& target);

	inline void SetKinematicTarget(const physx::PxVec3& targetPos, const physx::PxQuat& targetRot)
	{
		SetKinematicTarget(physx::PxTransform(targetPos, targetRot));
	}

	inline void SetKinematicTarget(const physx::PxVec3& targetPos)
	{
		SetKinematicTarget(targetPos, GetOrientation());
	}

	void SetLinearDamping(const float damping);

	void SetAngularDamping(const float damping);

	float GetLinearDamping()const;

	float GetAngularDamping()const;

	///<summary>Use this to only set rotation for a kinematic actor</summary>
	///<param name="targetRot">The target orientation</param>
	inline void SetKinematicTarget(const physx::PxQuat& targetRot)
	{
		SetKinematicTarget(GetPosition(), targetRot);
	}

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

