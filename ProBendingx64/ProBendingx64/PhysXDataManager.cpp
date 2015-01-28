#include "PhysXDataManager.h"
#include "PhysXCookingWrapper.h"
#include "PhysXSerializerWrapper.h"
#include "MeshInfo.h"
#include "ShapeDefinition.h"
#include "XMLWriter.h"
#include "XMLReader.h"

#include "NotImplementedException.h"

#include "geometry/PxTriangleMesh.h"
#include "geometry/PxHeightField.h"

#include "cooking/PxConvexMeshDesc.h"
#include "PxPhysics.h"
#include "extensions/PxDefaultStreams.h"

using namespace physx;

PhysXDataManager* PhysXDataManager::instance = NULL;

PhysXDataManager::PhysXDataManager(void)
{
}

PhysXDataManager::~PhysXDataManager(void)
{
	for (auto start = materialMap.begin(); start != materialMap.end(); ++start)
	{
		start->second->release();
	}

	materialMap.clear();

	for(auto start = shapeMap.begin(); start != shapeMap.end(); ++start)
	{
		start->second->release();
	}

	shapeMap.clear();

	for(auto start = convexMeshMap.begin(); start != convexMeshMap.end(); ++start)
	{
		start->second->release();
	}

	convexMeshMap.clear();

	for(auto start = triangleMeshMap.begin(); start != triangleMeshMap.end(); ++start)
	{
		start->second->release();
	}

	triangleMeshMap.clear();

	for(auto start = heightFieldMap.begin(); start != heightFieldMap.end(); ++start)
	{
		start->second->release();
	}

	heightFieldMap.clear();
}

physx::PxMaterial* PhysXDataManager::CreateMaterial(const physx::PxReal staticFriction, 
			const physx::PxReal dynamicFriction, const physx::PxReal restitution, 
			const std::string& name /*= std::string("")*/)
{
	MaterialMap::iterator hint = materialMap.begin();

	bool saveMaterial = !name.empty();

	if(saveMaterial)
	{
		if(FindExistingWithHint(name, hint))
			return hint->second;
	}

	physx::PxMaterial* newMat = PxGetPhysics().createMaterial(staticFriction, dynamicFriction, restitution);
	
	printf("Physics Material Count: %i\n", PxGetPhysics().getNbMaterials());

	if(newMat)
		if(saveMaterial)
			materialMap.insert(hint, MaterialMap::value_type(name, newMat));

	return newMat;

}

physx::PxConvexMesh* const PhysXDataManager::CookConvexMesh(std::shared_ptr<MeshInfo> meshInfo,
					const std::string& meshName)
{
	ConvexMeshMap::iterator hint = convexMeshMap.begin();//start at beginning so we can use the hint even if we don't search the map

	if(FindExistingWithHint(meshName, hint))
		// key already exists, return NULL to indicate cooking failed
		return NULL;

	physx::PxConvexMesh* convexMesh = NULL;

	//if cooking library has been enabled
	if(PhysXCookingWrapper::CookingValid())
	{
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count     = meshInfo->vertex_count;
		convexDesc.points.stride    = sizeof(Ogre::Vector3);
		convexDesc.points.data      = meshInfo->vertices;
		convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxDefaultMemoryOutputStream buf;
		physx::PxConvexMeshCookingResult::Enum result;

		//Cook the mesh into physx output stream
		if(PhysXCookingWrapper::cooking->cookConvexMesh(convexDesc, buf, &result))
		{
			//Create an input data from the cooked data and create mesh
			physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
			convexMesh = PxGetPhysics().createConvexMesh(input);
		}
	}
	
	if (convexMesh)//insert if saving with hint from above
		convexMeshMap.insert(hint, ConvexMeshMap::value_type(meshName, convexMesh));

	return convexMesh; 
}

physx::PxTriangleMesh* PhysXDataManager::CookTriangleMesh(std::shared_ptr<MeshInfo> meshInfo, std::string& name /*= std::string("")*/)
{
	throw NotImplementedException();
}

physx::PxHeightField* PhysXDataManager::CookHeightField(std::shared_ptr<MeshInfo> meshInfo, std::string& name /*= std::string("")*/)
{
	throw NotImplementedException();
}

physx::PxShape* PhysXDataManager::CreateShape(const ShapeDefinition& shapeDefinition, const std::string& name)
{
	ShapeMap::iterator hint = shapeMap.begin();//start at beginning so we can use the hint even if we don't search the map

	if(FindExistingWithHint(name, hint))
		// key already exists
		return hint->second;

	if(shapeDefinition.MaterialList.size() == 0)//if we are forced to create it, make sure we have valid structure
		return NULL;

	physx::PxShape* shape = PxGetPhysics().createShape(*shapeDefinition.ShapeGeometry.get(), 
		&shapeDefinition.MaterialList[0], (physx::PxU16)shapeDefinition.MaterialList.size(), 
		false, shapeDefinition.ShapeFlags);

	if(shape)
	{
		shape->setLocalPose(shapeDefinition.Transform);
		shapeMap.insert(hint, ShapeMap::value_type(name, shape));
	}

	return shape;
}

bool PhysXDataManager::SerializeData(const PxDataManSerializeOptions& serializationOptions)
{
	/*Serialization will go:
	1) Create objects in each collection with ID
	2) Serialize physX objects in binary
	3) Serialize ID's with names in XMLWriter by getting them from PhysX*/
	
	if(serializationOptions.SerializerDataTypes & PxDataManSerializeOptions::NONE)//Check for valid options
		return false;

	PxDataManSerializeOptions::DataSerializers options = serializationOptions.SerializerDataTypes;

	bool materialsSerialized = false, shapesSerialized = false, convexSerialized = false,
		triangleMeshSerialized = false, heightFieldSerialized = false;

	//Gather all desired components into the specified collection
	if(options & PxDataManSerializeOptions::MATERIALS)
		GatherMaterials(serializationOptions.CollectionName);
	if(options & PxDataManSerializeOptions::CONVEX_MESHES)
		GatherConvexMeshes(serializationOptions.CollectionName);
	if(options & PxDataManSerializeOptions::TRIANGLE_MESHES)
		GatherTriangleMeshes(serializationOptions.CollectionName);
	if(options & PxDataManSerializeOptions::HEIGHT_FIELD)
		GatherHeightFields(serializationOptions.CollectionName);
	if(options & PxDataManSerializeOptions::SHAPES)
		GatherShapes(serializationOptions.CollectionName);

	PhysXSerializerWrapper::CreateSerializer();//Add reference count or create

	unsigned short startID = 1;
		
	//Create IDs for all objects starting at one
	PhysXSerializerWrapper::CreateIDs(startID);

	//Try to serialize to binary. If failed, clean up
	if(!PhysXSerializerWrapper::SerializeToBinary(serializationOptions.FileName + ".pbd", 
				serializationOptions.CollectionName))
	{
		PhysXSerializerWrapper::ReleaseCollection(serializationOptions.CollectionName);
		PhysXSerializerWrapper::DestroySerializer();
		return false;
	}

	//Try to save list to XML
	bool XMLWritingResult = SaveXML(serializationOptions.FileName + ".xml", serializationOptions.CollectionName);
		
	//Clean up
	if(!serializationOptions.PersistantCollection)
		PhysXSerializerWrapper::ReleaseCollection(serializationOptions.CollectionName);
	PhysXSerializerWrapper::DestroySerializer();//Decrement reference count or destroy

	return XMLWritingResult;	
}

bool PhysXDataManager::DeserializeData(const PxDataManSerializeOptions& serializationOptions)
{
		/*Deserialization will go:
		1) Deserialize physX objects
		2) Deserialize XML file
		3) Add the shape with the corresponding ID to this Data Manager with the appropriate name
		4) Load and attach any Rigid Body Components
		How to serialize the required shape by the Rigid Body Component?
		-Rigid bodies will get the shape collection from this manager
		-Rigid body actors will be serialized with PhysX
		-XML serialization will take the rigid body actor PhysX Serial ID from collection and store it
		-Materials and Shapes will be excepted from the rigid body collection
		-On load, take that ID and try to find the actor associated with it, then assign it.
		--theoretically, this loaded actor should have its shape and everything loaded, and
		should be able to be assigned easily to the rigid body component, which will have any 
		additional data (additional to the serialization ID) stored in the custom XML format

		---Therefore, some sort of saving manager needs to take all game objects and save their rigid bodies
		*/
	PhysXSerializerWrapper::CreateSerializer();
	
	bool deserialResult = PhysXSerializerWrapper::DeserializeFromBinary(serializationOptions.FileName + ".pbd",
		serializationOptions.CollectionName);

	if(deserialResult)
	{
		deserialResult = LoadXML(serializationOptions.FileName + ".xml", 
			serializationOptions.CollectionName);
	}

	if(!serializationOptions.PersistantCollection)
		PhysXSerializerWrapper::ReleaseCollection(serializationOptions.CollectionName);
	PhysXSerializerWrapper::DestroySerializer();

	return deserialResult;
}

bool PhysXDataManager::SaveXML(const std::string& fileName, const std::string& collectionName)
{
	XMLWriter writer = XMLWriter();
	
	writer.CreateNode("PhysXDataManager", "", true);//Create root node
	writer.CreateNode("MaterialList", "", true); //Create material list root

	//Store these here so Rapid XML doesn't allocate them. As long as it is written to disk
	//before these go out of scope, everything will save properly
	std::string SerialID = "SerialID";
	std::string StorageName = "StorageName";

	std::string MaterialListItem = "MaterialListItem";
	std::string convexListItem = "ConvexListItem";
	std::string triangleListItem = "TriangleListItem";
	std::string heightFieldListItem = "HeightFieldListItem";
	std::string ShapeListItem = "ShapeListItem";

	for (MaterialMap::iterator start = materialMap.begin();
			start != materialMap.end(); ++start)
	{
		long long matID = PhysXSerializerWrapper::GetID(collectionName, *start->second);
		
		//if valid ID was found, add it
		if(matID > PX_SERIAL_OBJECT_ID_INVALID)
		{
			writer.CreateNode(MaterialListItem, std::string(""), true);//Place new material list item on the stack with no value
			writer.AddAttribute(SerialID, matID);//Add the Serial ID
			writer.AddAttribute(StorageName, start->first, false, false); //Add the name its stored under
			writer.PopNode();//Take it off the stack
		}
	}

	writer.PopNode();//Pop material list node off
	writer.CreateNode("ConvexMeshList", "", true);

	for (ConvexMeshMap::iterator start = convexMeshMap.begin();
		start != convexMeshMap.end(); ++start)
	{
		long long meshID = PhysXSerializerWrapper::GetID(collectionName, *start->second);

		if(meshID > PX_SERIAL_OBJECT_ID_INVALID)//if valid ID
		{
			writer.CreateNode(convexListItem, std::string(""), true);//Place new material list item on the stack with no value
			writer.AddAttribute(SerialID, meshID);//Add the Serial ID
			writer.AddAttribute(StorageName, start->first, false, false); //Add the name its stored under
			writer.PopNode();//Take it off the stack
		}
	}

	writer.PopNode();//Pop Convex Mesh list node off
	writer.CreateNode("TriangleMeshList", "", true);

	for (TriangleMeshMap::iterator start = triangleMeshMap.begin();
		start != triangleMeshMap.end(); ++start)
	{
		long long meshID = PhysXSerializerWrapper::GetID(collectionName, *start->second);

		if(meshID > PX_SERIAL_OBJECT_ID_INVALID)//if valid ID
		{
			writer.CreateNode(triangleListItem, std::string(""), true);//Place new material list item on the stack with no value
			writer.AddAttribute(SerialID, meshID);//Add the Serial ID
			writer.AddAttribute(StorageName, start->first, false, false); //Add the name its stored under
			writer.PopNode();//Take it off the stack
		}
	}

	writer.PopNode();//Pop Triangle Mesh list node off
	writer.CreateNode("HeightFieldList", "", true);

	for (HeightFieldMap::iterator start = heightFieldMap.begin();
		start != heightFieldMap.end(); ++start)
	{
		long long fieldID = PhysXSerializerWrapper::GetID(collectionName, *start->second);

		if(fieldID > PX_SERIAL_OBJECT_ID_INVALID)//if valid ID
		{
			writer.CreateNode(heightFieldListItem, std::string(""), true);//Place new height field list item on the stack with no value
			writer.AddAttribute(SerialID, fieldID);//Add the Serial ID
			writer.AddAttribute(StorageName, start->first, false, false); //Add the name its stored under
			writer.PopNode();//Take it off the stack
		}
	}

	writer.PopNode(); //Pop height field list node off
	writer.CreateNode("ShapeList", "", true);

	//Same as above material loop
	for (ShapeMap::iterator start = shapeMap.begin();
		start != shapeMap.end(); ++start)
	{
		long long shapeID = PhysXSerializerWrapper::GetID(collectionName, *start->second);

		if(shapeID > PX_SERIAL_OBJECT_ID_INVALID)
		{
			writer.CreateNode(ShapeListItem, std::string(""), true);
			writer.AddAttribute(SerialID, shapeID);
			writer.AddAttribute(StorageName, start->first, false, false);
			writer.PopNode();
		}
	}

	return writer.WriteFile(fileName);
}

bool PhysXDataManager::LoadXML(const std::string& fileName, const std::string& collectionName)
{
	XMLReader reader = XMLReader();

	if(!reader.OpenFile(fileName))
		return false;
	
	std::string SerialID = "SerialID";
	std::string StorageName = "StorageName";

	std::string MaterialListItem = "MaterialListItem";
	std::string convexListItem = "ConvexListItem";
	std::string triangleListItem = "TriangleListItem";
	std::string heightFieldListItem = "HeightFieldListItem";
	std::string ShapeListItem = "ShapeListItem";
	//<PhysXDataManager>
	//	<MaterialList>
	//	<MaterialListItem SerialID="1" StorageName="ProbenderMaterial"/>
	//	</MaterialList>
	//	<ShapeList/>
	//	</PhysXDataManager>
	do//Start at One below the root node
	{
		if(reader.MoveToChildNode())//Go to child (____ListItem)
		{
			long long serialID = 0;
			std::string storageName = "";

			do //loop through attributes of current node
			{
				if(reader.GetCurrentAttributeName() == SerialID)
					if(!reader.GetLongValue(serialID, true))
						serialID = 0;

				if(reader.GetCurrentAttributeName() == StorageName)
					storageName = reader.GetStringValue(true);

			} while (reader.MoveToNextAttribute());

			if(serialID != PX_SERIAL_OBJECT_ID_INVALID && !storageName.empty())
			{
				std::string currNodeName = reader.GetCurrentNodeName();

				//Check the current node name and generate the appropriate item
				if(currNodeName == MaterialListItem)
				{
					physx::PxMaterial* Material = (physx::PxMaterial*)PhysXSerializerWrapper::
						FindByID(collectionName, serialID);

					if(Material)
						materialMap.insert(MaterialMap::value_type(storageName, Material));
				}
				else if (currNodeName == convexListItem)
				{
					physx::PxConvexMesh* Convex = (physx::PxConvexMesh*)PhysXSerializerWrapper::
						FindByID(collectionName, serialID);
					if(Convex)
						convexMeshMap.insert(ConvexMeshMap::value_type(storageName, Convex));
				}
				else if (currNodeName == triangleListItem)
				{
					physx::PxTriangleMesh* TriMesh = (physx::PxTriangleMesh*)PhysXSerializerWrapper::
						FindByID(collectionName, serialID);
					if(TriMesh)
						triangleMeshMap.insert(TriangleMeshMap::value_type(storageName, TriMesh));
				}
				else if (currNodeName == heightFieldListItem)
				{
					physx::PxHeightField* heightField = (physx::PxHeightField*)PhysXSerializerWrapper::
						FindByID(collectionName, serialID);
					if(heightField)
						heightFieldMap.insert(HeightFieldMap::value_type(storageName, heightField));
				}
				else if(currNodeName == ShapeListItem)
				{
					physx::PxShape* shape = (physx::PxShape*)PhysXSerializerWrapper::
						FindByID(collectionName, serialID);

					if(shape)
						shapeMap.insert(ShapeMap::value_type(storageName, shape));
				}
			}
		}
		else//If no more children, move up a level
			reader.PopNode();
		
	} while (reader.MoveToNextSiblingNode());

	return true;
}

void PhysXDataManager::GatherMaterials(const std::string& collectionName)
{
	//if finding and setting the collection failed
	if(!PhysXSerializerWrapper::SetWorkingCollection(collectionName))
		//Create the collection
		PhysXSerializerWrapper::CreateCollection(collectionName, true);

	for (MaterialMap::iterator start = materialMap.begin();
		start != materialMap.end(); ++start)
	{
		PhysXSerializerWrapper::AddToWorkingCollection(*start->second);
	}

	PhysXSerializerWrapper::ClearWorkingCollection();
}

void PhysXDataManager::GatherConvexMeshes(const std::string& collectionName)
{
	//if finding and setting the collection failed
	if(!PhysXSerializerWrapper::SetWorkingCollection(collectionName))
		//Create the collection
			PhysXSerializerWrapper::CreateCollection(collectionName, true);

	//Add each mesh to the collection
	for (ConvexMeshMap::iterator start = convexMeshMap.begin();
		start != convexMeshMap.end(); ++start)
	{
		PhysXSerializerWrapper::AddToWorkingCollection(*start->second);
	}

	PhysXSerializerWrapper::ClearWorkingCollection();
}

void PhysXDataManager::GatherTriangleMeshes(const std::string& collectionName)
{
	//if finding and setting the collection failed
	if(!PhysXSerializerWrapper::SetWorkingCollection(collectionName))
		//Create the collection and set as working
			PhysXSerializerWrapper::CreateCollection(collectionName, true);

	//Add each mesh to the collection
	for (TriangleMeshMap::iterator start = triangleMeshMap.begin();
		start != triangleMeshMap.end(); ++start)
	{
		PhysXSerializerWrapper::AddToWorkingCollection(*start->second);
	}

	PhysXSerializerWrapper::ClearWorkingCollection();
}

void PhysXDataManager::GatherHeightFields(const std::string& collectionName)
{
	//if finding and setting the collection failed
	if(!PhysXSerializerWrapper::SetWorkingCollection(collectionName))
		//Create the collection
			PhysXSerializerWrapper::CreateCollection(collectionName, true);

	//Add each mesh to the collection
	for (HeightFieldMap::iterator start = heightFieldMap.begin();
		start != heightFieldMap.end(); ++start)
	{
		PhysXSerializerWrapper::AddToWorkingCollection(*start->second);
	}

	PhysXSerializerWrapper::ClearWorkingCollection();
}

void PhysXDataManager::GatherShapes(const std::string& collectionName, 
	const std::string& exceptForCollectionName /*= std::string("")*/, 
	bool includeMaterials /*= false*/, bool includeConvexMesh /*= false*/, 
	bool includeTriangleMesh /*= false*/, bool includeHeightField /*= false*/)
{
	//if finding and setting the collection failed
	if(!PhysXSerializerWrapper::SetWorkingCollection(collectionName))
		//Create the collection
			PhysXSerializerWrapper::CreateCollection(collectionName, true);

	for (ShapeMap::iterator start = shapeMap.begin();
		start != shapeMap.end(); ++start)
	{
		//Add the shape itself
		PhysXSerializerWrapper::AddToWorkingCollection(*start->second);

		switch (start->second->getGeometryType())
		{
		case PxGeometryType::eCONVEXMESH:
			if(includeConvexMesh)
			{
				physx::PxConvexMeshGeometry mesh;
				if(start->second->getConvexMeshGeometry(mesh))
					PhysXSerializerWrapper::AddToWorkingCollection(*mesh.convexMesh);
			}
			break;

		case PxGeometryType::eTRIANGLEMESH:
			if(includeTriangleMesh)//if triangle mesh should be included
			{
				physx::PxTriangleMeshGeometry mesh;
				if(start->second->getTriangleMeshGeometry(mesh))
					PhysXSerializerWrapper::AddToWorkingCollection(*mesh.triangleMesh);
			}
			break;
		case PxGeometryType::eHEIGHTFIELD:
			if(includeHeightField)
			{
				physx::PxHeightFieldGeometry field;
				if(start->second->getHeightFieldGeometry(field))
					PhysXSerializerWrapper::AddToWorkingCollection(*field.heightField);
			}
			break;
		default:
			break;
		}

		if(includeMaterials)//If materials should be included in the collection
		{
			physx::PxMaterial** materials = new physx::PxMaterial*[start->second->getNbMaterials()];
			unsigned int added = start->second->getMaterials(materials, start->second->getNbMaterials());

			for (unsigned int i = 0; i <added; ++i)
			{
				PhysXSerializerWrapper::AddToWorkingCollection(*materials[i]);
			}
		}
	}

	PhysXSerializerWrapper::CompleteCollection(collectionName, exceptForCollectionName);
	PhysXSerializerWrapper::ClearWorkingCollection();
}