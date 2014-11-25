#pragma once
#include "foundation\PxVec3.h"
#include "OgreVector3.h"

///<summary>Convenience function for converting an Ogre Vector3 to a physx Vector3</summary>
///<param "ogreVector">The ogre vector to convert</param>
///<returns>The physX vector3 representation</returns>
static inline physx::PxVec3 OgreToPhysXVec3(const Ogre::Vector3& ogreVector)
{
	return physx::PxVec3(ogreVector.x, ogreVector.y, ogreVector.z);
}

///<summary>Convenience function for converting a physx Vector3 to an Ogre Vector3</summary>
///<param "physxVector">The physX vector to convert</param>
///<returns>The ogre vector3 representation</returns>
static inline Ogre::Vector3 PhysXToOgreVec3(const physx::PxVec3& physxVector)
{
	return Ogre::Vector3(physxVector.x, physxVector.y, physxVector.z);
}