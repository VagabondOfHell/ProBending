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
	
	static std::wstring StringToWideString(const std::string& stringToConvert);

	static std::string WideStringToString(const std::wstring& stringToConvert);

	///<summary>Calculates the magnitude of the passed values.
	///Useful for structures such as CameraSpacePoint</summary>
	///<param name="x">The X Value of the 3 dimensional coordinate</param>
	///<param name="y">The Y Value of the 3 dimensional coordinate</param>
	///<param name="z">The Z Value of the 3 dimensional coordinate</param>
	///<returns>Number representing the length of the vector after square root</returns>
	static inline float CalculateMagnitude(float x, float y, float z)
	{
		return Ogre::Math::Sqrt(x * x + y * y + z * z);
	}

	///<summary>!AVOIDS CALLING SQUARE ROOT! Calculates the magnitude of the passed values.
	///Useful for structures such as CameraSpacePoint</summary>
	///<param name="x">The X Value of the 3 dimensional coordinate</param>
	///<param name="y">The Y Value of the 3 dimensional coordinate</param>
	///<param name="z">The Z Value of the 3 dimensional coordinate</param>
	///<returns>Number representing the length of the vector before square root</returns>
	static inline float CalculateMagnitudeSqrd(float x, float y, float z)
	{
		return x * x + y * y + z * z;
	}

	///<summary>Gets the decimal percentage of the values position between the min and max</summary>
	///<param name="minVal">The minimum allowed value</param>
	///<param name="maxVal">The maximum allowed value</param>
	///<param name="value">The value being converted to a percentage</param>
	///<returns>Decimal representing the percentage across the values</returns>
	static inline float CalculatePercentage(float minVal, float maxVal, float value)
	{
		//Bring all the values between 0 and positive #, then perform normal percentage equation (val/max)
		return (value - minVal) / (maxVal - minVal);
	}

	///<summary>Inverse of Calculate Percentage, calculates the actual value between a range given a percent</summary>
	///<param name="minVal">The lowest value allowed</param>
	///<param name="maxVal">The highest value allowed</param>
	///<param name="percentile">The percentile of the value to retrieve, in decimal form</param>
	///<returns>The value according to the given percentage</returns>
	static inline float CalculateValue(float minVal, float maxVal, float percentile)
	{
		//Bring the values between 0 and positive # (max - min) and multiply that by the percentile,
		//adding the result to the minimum to get the actual position
		return minVal + (percentile * (maxVal - minVal));
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