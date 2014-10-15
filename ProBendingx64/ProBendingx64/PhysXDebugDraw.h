#pragma once
#include "PxPhysics.h"
#include "OgreManualObject.h"

class PhysXDebugDraw
{
public: 

	static void DrawBoxGeometry(physx::PxVec3 shapePosition, physx::PxBoxGeometry* boxGeometry, Ogre::ManualObject* manualObject)
	{
		Ogre::Vector3 halfSize = Ogre::Vector3(boxGeometry->halfExtents.x, 
			boxGeometry->halfExtents.y, boxGeometry->halfExtents.z);

		//Back square vectors
		Ogre::Vector3 v1 = Ogre::Vector3(shapePosition.x - halfSize.x, shapePosition.y + halfSize.y, shapePosition.z - halfSize.z);
		Ogre::Vector3 v2 = Ogre::Vector3(shapePosition.x + halfSize.x, shapePosition.y + halfSize.y, shapePosition.z - halfSize.z);
		Ogre::Vector3 v3 = Ogre::Vector3(shapePosition.x + halfSize.x, shapePosition.y - halfSize.y, shapePosition.z - halfSize.z);
		Ogre::Vector3 v4 = Ogre::Vector3(shapePosition.x - halfSize.x, shapePosition.y - halfSize.y, shapePosition.z - halfSize.z);

		//Front square vectors
		Ogre::Vector3 v5 = Ogre::Vector3(shapePosition.x - halfSize.x, shapePosition.y + halfSize.y, shapePosition.z + halfSize.z);
		Ogre::Vector3 v6 = Ogre::Vector3(shapePosition.x + halfSize.x, shapePosition.y + halfSize.y, shapePosition.z + halfSize.z);
		Ogre::Vector3 v7 = Ogre::Vector3(shapePosition.x + halfSize.x, shapePosition.y - halfSize.y, shapePosition.z + halfSize.z);
		Ogre::Vector3 v8 = Ogre::Vector3(shapePosition.x - halfSize.x, shapePosition.y - halfSize.y, shapePosition.z + halfSize.z);

		// specify the material (by name) and rendering type
		manualObject->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

		//Back face
		manualObject->position(v1);
		manualObject->position(v2);
		manualObject->position(v3);
		manualObject->position(v4);
		manualObject->position(v1);

		//Top Face
		manualObject->position(v1);
		manualObject->position(v2);
		manualObject->position(v6);
		manualObject->position(v5);
		manualObject->position(v1);

		//Front Face
		manualObject->position(v5);
		manualObject->position(v6);
		manualObject->position(v7);
		manualObject->position(v8);
		manualObject->position(v5);

		//Bottom Face
		manualObject->position(v4);
		manualObject->position(v3);
		manualObject->position(v7);
		manualObject->position(v8);
		manualObject->position(v4);

		//Right Face
		manualObject->position(v6);
		manualObject->position(v2);
		manualObject->position(v3);
		manualObject->position(v7);
		manualObject->position(v6);
		
		//Left Face
		manualObject->position(v5);
		manualObject->position(v1);
		manualObject->position(v4);
		manualObject->position(v8);
		manualObject->position(v5);

		manualObject->end();
	}
};