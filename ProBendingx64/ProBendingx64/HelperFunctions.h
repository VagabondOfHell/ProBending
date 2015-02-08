#pragma once
#include "foundation\PxVec3.h"
#include "OgreVector3.h"
#include "MeshInfo.h"

namespace physx
{
	class PxBoxGeometry;
};


class HelperFunctions
{
public:
	///<summary>Convenience function for converting an Ogre Vector3 to a physx Vector3</summary>
	///<param name="ogreVector">The ogre vector to convert</param>
	///<returns>The physX vector3 representation</returns>
	static inline physx::PxVec3 OgreToPhysXVec3(const Ogre::Vector3& ogreVector)
	{
		return physx::PxVec3(ogreVector.x, ogreVector.y, ogreVector.z);
	}

	///<summary>Convenience function for converting a physx Vector3 to an Ogre Vector3</summary>
	///<param name="physxVector">The physX vector to convert</param>
	///<returns>The ogre vector3 representation</returns>
	static inline Ogre::Vector3 PhysXToOgreVec3(const physx::PxVec3& physxVector)
	{
		return Ogre::Vector3(physxVector.x, physxVector.y, physxVector.z);
	}

	///<summary>Fills the manual object with the vertices required to render a box around 
	///a physX box</summary>
	///<param name="shapePosition">The position of the physX shape/object</param>
	///<param name="boxGeometry">The physx geometry</param>
	///<param name="manualObject">The Ogre Manual to fill with draw data</param>
	static void DrawBoxGeometry(const physx::PxVec3 shapePosition, 
		const physx::PxBoxGeometry* const boxGeometry, Ogre::ManualObject* manualObject);

	static void GetMeshInformation(const Ogre::Mesh* const mesh, MeshInfo& meshInfo,
		const Ogre::Vector3 &position = Ogre::Vector3::ZERO, const Ogre::Quaternion &orient = Ogre::Quaternion::IDENTITY, 
		const Ogre::Vector3 &scale = Ogre::Vector3::UNIT_SCALE);
};