#include "HelperFunctions.h"
#include "geometry\PxBoxGeometry.h"
#include "OgreManualObject.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"

#include "foundation/PxQuat.h"

#include <codecvt>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef DEGREES_TO_RADIANS
#define DEGREES_TO_RADIANS (M_PI / 180.0f)
#endif 

void HelperFunctions::DrawBoxGeometry(const physx::PxVec3 shapePosition, 
			const physx::PxBoxGeometry* const boxGeometry, Ogre::ManualObject* manualObject)
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


void HelperFunctions::GetMeshInformation(const Ogre::Mesh* const mesh, MeshInfo& meshInfo, 
			const Ogre::Vector3 &position /*= Ogre::Vector3::ZERO*/, const Ogre::Quaternion &orient /*= Ogre::Quaternion::IDENTITY*/, 
			const Ogre::Vector3 &scale /*= Ogre::Vector3::UNIT_SCALE*/)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	meshInfo.vertex_count = meshInfo.index_count = 0;

	// Calculate how many vertices and indices we're going to need
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);
		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
			{
				meshInfo.vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			meshInfo.vertex_count += submesh->vertexData->vertexCount;
		}
		// Add the indices
		meshInfo.index_count += submesh->indexData->indexCount;
	}

	// Allocate space for the vertices and indices
	meshInfo.vertices = new Ogre::Vector3[meshInfo.vertex_count];
	meshInfo.indices = new unsigned long[meshInfo.index_count];

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf =
				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//Ogre::Real* pReal;
			float* pReal;

			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);
				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);
				meshInfo.vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}

		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long* pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				meshInfo.indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				meshInfo.indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
					static_cast<unsigned long>(offset);
			}
		}

		ibuf->unlock();
		current_offset = next_offset;
	}

	meshInfo.aaBB = mesh->getBounds();
	meshInfo.aaBB.scale(scale);

	meshInfo.sphereRadius = Ogre::Math::Abs(position.distance(position - meshInfo.aaBB.getMaximum()));
}

std::wstring HelperFunctions::StringToWideString(const std::string& stringToConvert)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(stringToConvert);
}

std::string HelperFunctions::WideStringToString(const std::wstring& stringToConvert)
{
	typedef std::codecvt_utf8<wchar_t> convert_typeX;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(stringToConvert);
}

physx::PxQuat HelperFunctions::FromEulerAngles(const physx::PxVec3& angles)
{
	float roll = angles.x * DEGREES_TO_RADIANS;
	float pitch = angles.y * DEGREES_TO_RADIANS;
	float yaw = angles.z * DEGREES_TO_RADIANS;

	float cyaw, cpitch, croll, syaw, spitch, sroll;
	float cyawcpitch, syawspitch, cyawspitch, syawcpitch;

	cyaw = cosf(0.5f * yaw);
	cpitch = cosf(0.5f * pitch);
	croll = cosf(0.5f * roll);
	syaw = sinf(0.5f * yaw);
	spitch = sinf(0.5f * pitch);
	sroll = sinf(0.5f * roll);

	cyawcpitch = cyaw * cpitch;
	syawspitch = syaw * spitch;
	cyawspitch = cyaw * spitch;
	syawcpitch = syaw * cpitch;

	return physx::PxQuat(
		cyawcpitch * sroll - syawspitch * croll,
		cyawspitch * croll + syawcpitch * sroll,
		syawcpitch * croll - cyawspitch * sroll,
		cyawcpitch * croll + syawspitch * sroll);
}
