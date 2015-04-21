#include "SceneSerializer.h"

#include "PhysXDataManager.h"
#include "PhysXSerializerWrapper.h"

#include "NotImplementedException.h"

#include "IScene.h"

#include "Component.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"
#include "ParticleComponent.h"

#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"

#include "ParticleSystemBase.h"
#include "ParticlePointEmitter.h"
#include "ColourFadeParticleAffector.h"
#include "ParticleAffectors.h"
#include "ParticleAffectorEnum.h"

#include "OgreLogManager.h"
#include "OgreEntity.h"
#include "OgreSubEntity.h"

typedef std::unordered_set<SharedGameObject> GameObjectList;

SceneSerializer::SceneSerializer()
	: StartID(1), ActorCollection("ActorCollection"),
	GameObjectNode("GameObjectNode"), ObjectName("ObjectName"), ObjectTag("ObjectTag"),
	Position("Position"), X("X"), Y("Y"), Z("Z"), W("W"), Rotation("Rotation"), Scale("Scale"),
	MeshRenderComponen("MeshRenderComponent"), RigidBodyComponen("RigidBodyComponent"),
	ParticleComponen("ParticleComponent"), Enabled("Enabled"), EntityName("Entity"), 
	SubentityMaterial("SubentityMaterial"), Subentity("Subentity"), SubentityID("SubentityID"),
	RigidShape("RigidShape"), ShapeName("ShapeName"), DynamicBodyType("DynamicBodyType"),
	ParticleSpace("ParticleSpace"), Emitter("Emitter"), PointEmitter("PointEmitter"), LineEmitter("LineEmitter"), 
	MeshEmitter("MeshEmitter"), MinEmitDirection("MinDirection"), MaxEmitDirection("MaxDirection"), 
	PPS("ParticlesPerSecond"), MinEmitSpeed("MinSpeed"), MaxEmitSpeed("MaxSpeed"), Duration("Duration"),
	ParticleActorID("ParticleActorID"), MaxParticles("MaxParticles"), OnGPU("OnGPU"), EmitLoop("Loop"),
	InitialLifetime("InitialLifetime"), ParticleAffectors("ParticleAffectors"), ScaleAffector("ScaleAffector"), 
	ColourFaderAffector("ColourFadeAffector"), AlphaFadeAffector("AlphaFadeAffector"), Enlarge("Enlarge"), 
	MinScale("MinScale"), MaxScale("MaxScale"), StartColour("StartColour"), EndColour("EndColour")
{

}

SceneSerializer::~SceneSerializer()
{

}

#pragma region Serialization

bool SceneSerializer::AddVector3Attribute(XMLWriter& writer, const float x, const float y, const float z)
{
	if(writer.AddAttribute(X, x))
		if(writer.AddAttribute(Y, y))
			if(writer.AddAttribute(Z, z))
				return true;

	return false;
}

bool SceneSerializer::AddVector3Attribute(XMLWriter& writer, physx::PxVec3 vec)
{
	return AddVector3Attribute(writer, vec.x, vec.y, vec.z);
}

bool SceneSerializer::AddVector3Attribute(XMLWriter& writer, Ogre::Vector3 vec)
{
	return AddVector3Attribute(writer, vec.x, vec.y, vec.z);
}

bool SceneSerializer::AddVector4Attribute(XMLWriter& writer, const float x, 
		const float y, const float z, const float w)
{
	if(writer.AddAttribute(X, x))
		if(writer.AddAttribute(Y, y))
			if(writer.AddAttribute(Z, z))
				if(writer.AddAttribute(W, w))
					return true;

	return false;
}

bool SceneSerializer::AddVector4Attribute(XMLWriter& writer, physx::PxVec4 vec)
{
	return AddVector4Attribute(writer, vec.x, vec.y, vec.z, vec.w);
}

bool SceneSerializer::AddVector4Attribute(XMLWriter& writer, Ogre::Vector4 vec)
{
	return AddVector4Attribute(writer, vec.x, vec.y, vec.z, vec.w);
}

bool SceneSerializer::SerializeScene(const IScene* scene, std::string rootNodeName, const std::string& fileName,
			const std::string& refCollection/* = std::string("")*/)
{
	//PxDataManSerializeOptions serialOptions = PxDataManSerializeOptions(PxDataManSerializeOptions::ALL,
		//PxDataCollection, true, fileName);
	StartID = PhysXSerializerWrapper::GetHighestIDInCollection(ActorCollection);

	pxDataRefCollection = refCollection;

	if(StartID < 1)
		StartID = 1;

	long long otherCollectionID = PhysXSerializerWrapper::GetHighestIDInCollection(refCollection);

	if(otherCollectionID < 1)
		otherCollectionID = 1;

	if(otherCollectionID > StartID)
		StartID = otherCollectionID + 1;

	if(PhysXSerializerWrapper::CreateSerializer() )//&& 
		//PhysXDataManager::GetSingletonPtr()->SerializeData(serialOptions))
	{
		XMLWriter writer;
	
		rootNodeName.erase(std::remove_if(rootNodeName.begin(), rootNodeName.end(), isspace), rootNodeName.end());

		if(	PhysXSerializerWrapper::CreateCollection(ActorCollection, true))
		{
			writer.CreateNode(rootNodeName, std::string(""), false);//Root node which is scene name

			for (GameObjectList::iterator start = scene->gameObjectList.begin();
				start != scene->gameObjectList.end(); ++start)
			{
				if(start->get()->IsSerializable())//if serializable, serialize
					SerializeGameObject(writer, *start);
			}

			//PhysXSerializerWrapper::CompleteCollection(ActorCollection, refCollection);

			bool serialize = PhysXSerializerWrapper::SerializeToBinary(fileName + "Actors.pbd",
				ActorCollection, refCollection);

			bool xml = writer.WriteFile(fileName + "Actors.xml");

			PhysXSerializerWrapper::ReleaseCollection(ActorCollection);
			PhysXSerializerWrapper::DestroySerializer();

			return serialize && xml;
		}		
	}

	return false;
}

void SceneSerializer::SerializeGameObject(XMLWriter& writer, const SharedGameObject gameObject)
{
	Ogre::Vector3 position = gameObject->GetWorldPosition();
	Ogre::Vector3 scale = gameObject->GetWorldScale();
	Ogre::Quaternion rotation = gameObject->GetWorldOrientation();
	
	writer.CreateNode(GameObjectNode, std::string(""), true); writer.AddAttribute(ObjectName, gameObject->GetName());
	writer.AddAttribute(ObjectTag, gameObject->tag, false, false);

		//game object Transform
		writer.CreateNode(Position);
			AddVector3Attribute(writer, position);
		writer.PopNode();

		writer.CreateNode(Rotation, gameObject->GetInheritOrientation());
			AddVector4Attribute(writer, rotation.x, rotation.y, rotation.z, rotation.w);
		writer.PopNode();

		writer.CreateNode(Scale, gameObject->GetInheritScale());
			AddVector3Attribute(writer, scale);
		writer.PopNode();

	for (auto compIter = gameObject->components.begin(); compIter != gameObject->components.end(); ++compIter)
	{
		switch (compIter->second->GetComponentType())
		{
		case Component::AUDIO_COMPONENT:
			break;

		case Component::MESH_RENDER_COMPONENT:
			SerializeMeshRenderComponent(writer, (MeshRenderComponent*)compIter->second);
			break;

		case Component::PARTICLE_COMPONENT:
			SerializeParticleComponent(writer, (ParticleComponent*)compIter->second);
			break;

		case Component::RIGID_BODY_COMPONENT:
			SerializeRigidBodyComponent(writer, (RigidBodyComponent*)compIter->second);
			break;

		default:
			break;
		}
	}

	//Serialize Child Game Objects
	for (auto childStart = gameObject->children.begin();
		childStart != gameObject->children.end(); ++childStart)
	{
		SerializeGameObject(writer, *childStart);
	}

	writer.PopNode();
}

bool SceneSerializer::SerializeMeshRenderComponent(XMLWriter& writer, const MeshRenderComponent* renderer)
{
	//create render component node, setting enabled as its value
	if(writer.CreateNode(MeshRenderComponen))
	{
		writer.AddAttribute(Enabled, renderer->IsEnabled());
		writer.AddAttribute(EntityName, renderer->GetMeshName(), false, false);

		long long numSubmesh = renderer->entity->getNumSubEntities();

		for (long long i = 0; i < numSubmesh; i++)
		{
			writer.CreateNode(Subentity);
				writer.AddAttribute(SubentityID, i);
				writer.AddAttribute(SubentityMaterial, renderer->entity->getSubEntity(i)->getMaterialName());
			writer.PopNode();
		}

		writer.PopNode();
		return true;
	}

	return false;
}

bool SceneSerializer::SerializeRigidBodyComponent(XMLWriter& writer, const RigidBodyComponent* rigidBody)
{
	using namespace physx;

	RigidBodyComponent::RigidBodyType rigidType = rigidBody->GetBodyType();

	if(rigidType == RigidBodyComponent::NONE)
		return false;

	//if rigid body node created
	if(writer.CreateNode(RigidBodyComponen))
	{
		//write enabled state
		writer.AddAttribute(Enabled, rigidBody->IsEnabled());
		writer.AddAttribute(DynamicBodyType, rigidBody->GetBodyType() == RigidBodyComponent::DYNAMIC);

		physx::PxRigidActor* actor = NULL;
		if(rigidBody->GetBodyType() == RigidBodyComponent::DYNAMIC)
			actor = rigidBody->GetDynamicActor();
		else if(rigidBody->GetBodyType() == RigidBodyComponent::STATIC)
			actor = rigidBody->GetStaticActor();

		//if no actor, pop off the rigid body component
		if(!actor)
			writer.PopNode();

		std::string shapeName = "";
		PxU32 numShapes = actor->getNbShapes();
		PxShape** shapes = new physx::PxShape*[numShapes];
		actor->getShapes(shapes, numShapes);

		//loop through the shapes
		for (PxU32 i = 0; i < numShapes; i++)
		{
			//if the shape can't be found, move next iteration
			if(!PhysXDataManager::GetSingletonPtr()->FindShapeName(shapes[i], shapeName))
				continue;
			
			//Create the shape node and store its reference
			writer.CreateNode(RigidShape);
			writer.AddAttribute(ShapeName, shapeName);
			writer.PopNode();
		}

		delete[] shapes;
	}
		
	writer.PopNode();

	return true;
}

bool SceneSerializer::SerializeParticleComponent(XMLWriter& writer, const ParticleComponent* particles)
{
	if(writer.CreateNode(ParticleComponen))
	{
		writer.AddAttribute(Enabled, particles->IsEnabled());
		writer.AddAttribute(ParticleSpace, particles->GetTransformationSpace(), false);//Store transformation space
		
		FluidAndParticleBase* particleSystem = particles->particleSystem;
		particleSystem->particleBase->releaseParticles();

		//Add particle system actor to the collection and store the ID in our format
		if(PhysXSerializerWrapper::AddToWorkingCollection(*particleSystem->particleBase))
		{
			PhysXSerializerWrapper::CreateIDs(ActorCollection, StartID);
			long long systemID = PhysXSerializerWrapper::GetID(ActorCollection, 
				*particleSystem->particleBase);

			if(systemID > PX_SERIAL_OBJECT_ID_INVALID)
				writer.AddAttribute(ParticleActorID, systemID);
		}
		//Particle system attributes
		writer.AddAttribute(MaxParticles, (long long)particleSystem->maximumParticles);
		writer.AddAttribute(OnGPU, particleSystem->onGPU);
		writer.AddAttribute(InitialLifetime, particleSystem->initialLifetime);

		bool emitterSerialized = SerializeParticleEmitter(writer, particles);

		bool affectorsSerialized = SerializeParticleAffectors(writer, particles);

		writer.PopNode();
		
		return emitterSerialized && affectorsSerialized;
	}

	return false;
}

bool SceneSerializer::SerializeParticleEmitter(XMLWriter& writer, const ParticleComponent* particles)
{
	std::shared_ptr<AbstractParticleEmitter> emitter = particles->particleSystem->GetEmitter();

	//Next add emitter
	AbstractParticleEmitter::ParticleEmitterType emitterType = emitter->GetEmitterType();

	bool emitterNodeCreated = false;

	//Create node and add any type specific attributes required
	switch (emitterType)
	{
	case AbstractParticleEmitter::POINT_EMITTER:
		emitterNodeCreated = writer.CreateNode(PointEmitter);
		break;
	case AbstractParticleEmitter::LINE_EMITTER:
		emitterNodeCreated = writer.CreateNode(LineEmitter);
		break;
	case AbstractParticleEmitter::MESH_EMITTER:
		emitterNodeCreated = writer.CreateNode(MeshEmitter);
		break;
	}

	//write abstract emitter properties
	if(emitterNodeCreated)
	{
		writer.AddAttribute(PPS, emitter->particlesPerSecond);
		writer.AddAttribute(MinEmitSpeed, emitter->minSpeed);
		writer.AddAttribute(MaxEmitSpeed, emitter->maxSpeed);
		writer.AddAttribute(Duration, emitter->duration);
		writer.CreateNode(Position);
		AddVector3Attribute(writer, emitter->position);
		writer.PopNode();

		writer.CreateNode(MinEmitDirection);
		AddVector3Attribute(writer, emitter->minimumDirection);
		writer.PopNode();

		writer.CreateNode(MaxEmitDirection);
		AddVector3Attribute(writer, emitter->maximumDirection);
		writer.PopNode();

		writer.PopNode();//Pop off emitter node
		return true;
	}

	return false;
}

bool SceneSerializer::SerializeParticleAffectors(XMLWriter& writer, const ParticleComponent* particles)
{
	ParticleSystemBase* particleSystem = (ParticleSystemBase*)particles->particleSystem;

	if(particleSystem->affectors.affectorMap.size() > 0)
		writer.CreateNode(ParticleAffectors);
	else
		return true;//if none to process, return as if successful

	for (auto affectIter = particleSystem->affectors.GetMapBegin();
		affectIter != particleSystem->affectors.GetMapEnd(); ++affectIter)
	{
		ParticleAffectorType::ParticleAffectorType affectType = 
			affectIter->second->GetAffectorType();

		switch (affectType)
		{
		case ParticleAffectorType::Scale:
			{
				if(writer.CreateNode(ScaleAffector))
				{
					GPUScaleAffectorParams* params = 
						(GPUScaleAffectorParams*)affectIter->second->GetGPUParamaters();

					writer.AddAttribute(OnGPU, affectIter->second->GetOnGPU());
					writer.AddAttribute(Enlarge,params->enlarge);
					writer.AddAttribute(MinScale, params->minScale);
					writer.AddAttribute(MaxScale, params->maxScale);

					writer.PopNode();
				}
			}
			
			break;
		case ParticleAffectorType::AlphaFade:
			break;
		case ParticleAffectorType::ColourToColour:
			{
				if(writer.CreateNode(ColourFaderAffector))
				{
					GPUColourFaderAffectorParams* params =
						(GPUColourFaderAffectorParams*)affectIter->second->GetGPUParamaters();

					writer.AddAttribute(OnGPU, affectIter->second->GetOnGPU());
					//Add start colour
					writer.CreateNode(StartColour);
					AddVector4Attribute(writer, params->startColour);
					writer.PopNode();

					//Add end colour
					writer.CreateNode(EndColour);
					AddVector4Attribute(writer, params->endColour);
					writer.PopNode();

					//pop off scale affector
					writer.PopNode();
				}
			}
			break;
		}
	}

	writer.PopNode();
	return true;
}

bool SceneSerializer::SerializeAudioComponent(XMLWriter& writer)
{
	throw NotImplementedException();
}

#pragma endregion 

bool SceneSerializer::DeserializeVector3(XMLReader& reader, Ogre::Vector3& retVal)
{
	float x, y, z;

	bool result = DeserializeVector3(reader, x, y, z);
	
	if(result)
		retVal = Ogre::Vector3(x, y, z);

	return result;
}

bool SceneSerializer::DeserializeVector3(XMLReader& reader, physx::PxVec3& retVal)
{
	float x, y, z;

	bool result = DeserializeVector3(reader, x, y, z);

	if(result)
		retVal = physx::PxVec3(x, y, z);

	return result;
}

bool SceneSerializer::DeserializeVector3(XMLReader& reader, float& outX, float& outY, float &outZ)
{
	bool result = false;
	float attVal = 0.0f;
	std::string currName;

	do 
	{
		currName = reader.GetCurrentAttributeName();

		if(currName == X)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outX = attVal;
		}
		else if(currName == Y)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outY = attVal;
		}
		else if(currName == Z)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outZ = attVal;
		}
		else
			result = false;

		if(result == false)
			break;
	} while (reader.MoveToNextAttribute());

	return result;
}

bool SceneSerializer::DeserializeVector4(XMLReader& reader, Ogre::Vector4& retVal)
{
	float x, y, z, w;

	bool result = DeserializeVector4(reader, x, y, z, w);

	if(result)
		retVal = Ogre::Vector4(x, y, z, w);

	return result;
}

bool SceneSerializer::DeserializeVector4(XMLReader& reader, physx::PxVec4& retVal)
{
	float x, y, z, w;

	bool result = DeserializeVector4(reader, x, y, z, w);

	if(result)
		retVal = physx::PxVec4(x, y, z, w);

	return result;
}

bool SceneSerializer::DeserializeVector4(XMLReader& reader, float& outX, float& outY, float &outZ, float &outW)
{
	bool result = false;
	float attVal = 0.0f;
	std::string currName;

	do 
	{
		currName = reader.GetCurrentAttributeName();

		if(currName == X)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outX = attVal;
		}
		else if(currName == Y)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outY = attVal;
		}
		else if(currName == Z)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outZ = attVal;
		}
		else if(currName == W)
		{
			result = reader.GetFloatValue(attVal, true);
			if(result)
				outW = attVal;
		}
		else
			result = false;

		if(result == false)
			break;
	} while (reader.MoveToNextAttribute());

	return result;
}

bool SceneSerializer::DeserializeScene(IScene* scene, const std::string& fileName, 
			const std::string& refCollection/* = std::string("")*/, bool flushDataMan /*=false*/, bool flushScene/* = false*/)
{
	/*PxDataManSerializeOptions serialOptions = PxDataManSerializeOptions(PxDataManSerializeOptions::ALL,
	PxDataCollection, true, fileName);*/

	bool errorsDetected = false;

	if(flushDataMan)
		PhysXDataManager::GetSingletonPtr()->ReleaseAll();

	if(flushScene)
		scene->GetPhysXScene()->flushSimulation();

	errorsDetected = !PhysXSerializerWrapper::CreateSerializer();
	/*
	if(!errorsDetected)
	errorsDetected = !PhysXDataManager::GetSingletonPtr()->DeserializeData(serialOptions);

	if(!errorsDetected)
	errorsDetected = !PhysXSerializerWrapper::AddToScene(scene->GetPhysXScene(), PxDataCollection);*/

	if(!errorsDetected)
		errorsDetected = !PhysXSerializerWrapper::DeserializeFromBinary(fileName + "Actors.pbd", 
			ActorCollection, refCollection);

	//PhysXSerializerWrapper::CompleteCollection(ActorCollection, refCollection);

	//if(!errorsDetected)
	//	errorsDetected = !PhysXSerializerWrapper::AddToScene(scene->GetPhysXScene(), ActorCollection);
	
	XMLReader reader = XMLReader();

	errorsDetected = !reader.OpenFile(fileName + "Actors.xml");

	if(!errorsDetected)
	{
		std::string currName;

		do 
		{
			currName = reader.GetCurrentNodeName();

			if(currName == GameObjectNode)//If dissecting a game object
			{
				//try loading the object and if successful, add it to the scene
				SharedGameObject loadedObject = DeserializedGameObject(reader, scene);
				if(loadedObject)
				{
					scene->AddGameObject(loadedObject);
					loadedObject->Start();
				}
				else
					errorsDetected = true;
			}
		} while (reader.MoveToNextSiblingNode());
	}
	
	//PhysXSerializerWrapper::ReleaseCollection(ActorCollection);
	PhysXSerializerWrapper::DestroySerializer();
	
	return !errorsDetected;
}

SharedGameObject SceneSerializer::DeserializedGameObject(XMLReader& reader, IScene* const scene)
{
	SharedGameObject newObject = SharedGameObject(new GameObject(scene));

	std::string currNode = reader.GetCurrentNodeName();

	//Handle game object attributes
	if(reader.NodeHasAttributes())
	{
		std::string currAttr;

		do 
		{
			currAttr = reader.GetCurrentAttributeName();

			if(currAttr == ObjectName)
				newObject->name = reader.GetStringValue(true);

			if(currAttr == ObjectTag)
				newObject->tag = reader.GetStringValue(true);

		} while (reader.MoveToNextAttribute());
	}

	Ogre::Vector3 objectPosition = Ogre::Vector3(0.0f);
	Ogre::Quaternion objectRotation = Ogre::Quaternion::IDENTITY;
	Ogre::Vector3 objectScale = Ogre::Vector3(1.0f);

	if(reader.MoveToChildNode())
	{
		do 
		{
			currNode = reader.GetCurrentNodeName();

			if(currNode == Position)
			{
				if(DeserializeVector3(reader, objectPosition))
					newObject->SetWorldPosition(objectPosition);
			}
			else if(currNode == Rotation)
			{
				if(!DeserializeVector4(reader, objectRotation.x,
					objectRotation.y, objectRotation.z, objectRotation.w))
					printf("Rot Fail\n");
				else
					newObject->SetWorldOrientation(objectRotation);

				bool inheritRot = true;

				if(reader.GetBoolValue(inheritRot, false))
					newObject->SetInheritOrientation(inheritRot);
			}
			else if(currNode == Scale)
			{
				if(DeserializeVector3(reader, objectScale))
					newObject->SetScale(objectScale);

				bool inheritScale = true;

				if(reader.GetBoolValue(inheritScale, false))
					newObject->SetInheritScale(inheritScale);
			}
			//Handle game object components
			else if(currNode == MeshRenderComponen)
				//Not worried about return type because component attaches to game object anyways
					//and errors get logged into Log Manager
						DeserializeMeshRenderComponent(reader, newObject);
			else if(currNode == RigidBodyComponen)
			{
				if(!DeserializeRigidBodyComponent(reader, newObject))
					printf("Rigid Body Failed\n");
			}
			else if(currNode == ParticleComponen)
			{
				DeserializeParticleComponent(reader, newObject);
			}
			else if(currNode == GameObjectNode)
			{
				SharedGameObject childObject = DeserializedGameObject(reader, scene);
				if(childObject)
					newObject->AddChild(childObject);
			}
			
		} while (reader.MoveToNextSiblingNode());
	}

	reader.PopNode();

	newObject->SetWorldTransform(objectPosition, objectRotation, objectScale);

	return newObject;
}

bool SceneSerializer::DeserializeMeshRenderComponent(XMLReader& reader, SharedGameObject& objectToAdd)
{
	if(reader.NodeHasAttributes())
	{
		MeshRenderComponent* mesh = new MeshRenderComponent();

		//Attach even if model loading failed
		objectToAdd->AttachComponent(mesh);

		bool modelLoaded = false;
		std::string currAttr;

		do 
		{
			currAttr = reader.GetCurrentAttributeName();
			if(currAttr == Enabled)
			{
				//Assume enabled is true unless explicitly otherwise
				bool enabledVal = true;
				reader.GetBoolValue(enabledVal, true);
				mesh->enabled = enabledVal;
			}
			else if(currAttr == EntityName)
			{
				modelLoaded = mesh->LoadModel(reader.GetStringValue(true));
				//If errors loading model, Log it
				if(!modelLoaded)
					Ogre::LogManager::getSingletonPtr()->getDefaultLog()->logMessage(
						"Mesh Render Component Deserialization Error: Mesh Not Loaded", Ogre::LML_CRITICAL);
			}
		} while (reader.MoveToNextAttribute());

		if(reader.MoveToChildNode())
		{
			do 
			{
				long long currentID = -1;
				std::string currentMaterial = "";

				do 
				{
					if (reader.GetCurrentAttributeName() == SubentityID)
					{
						if(!reader.GetLongValue(currentID, true))
							currentID = -1;
					}
					else if(reader.GetCurrentAttributeName() == SubentityMaterial)
						currentMaterial = reader.GetStringValue(true);

					if(currentID != -1 && currentMaterial != "")
						mesh->entity->getSubEntity(currentID)->setMaterialName(currentMaterial);

				} while (reader.MoveToNextAttribute());

			} while (reader.MoveToNextSiblingNode());
		}

		reader.PopNode();

		return modelLoaded;
	}

	return false;
}

bool SceneSerializer::DeserializeRigidBodyComponent(XMLReader& reader, SharedGameObject& objectToAdd)
{
	if(reader.NodeHasAttributes())
	{
		std::string currName;
		RigidBodyComponent* rigid = new RigidBodyComponent();
		objectToAdd->AttachComponent(rigid);

		bool success = true;
		bool rigidCreated = false;
		do 
		{
			currName = reader.GetCurrentAttributeName();

			if(currName == Enabled)
			{
				bool enabVal = false;
				if(reader.GetBoolValue(enabVal, true))
					rigid->enabled = enabVal;
				else
					success = false;
			}

			if(currName == DynamicBodyType)
			{
				bool dynamic = false;
				if(reader.GetBoolValue(dynamic, true))
				{
					if(dynamic)
						rigid->CreateRigidBody(RigidBodyComponent::DYNAMIC);
					else
						rigid->CreateRigidBody(RigidBodyComponent::STATIC, 
							HelperFunctions::OgreToPhysXVec3(objectToAdd->GetWorldPosition()));
					rigidCreated = true;
				}
				else
					success = false;
			}
		} while (reader.MoveToNextAttribute());

		//if rigid body wasn't created, don't try to add shapes
		if(!rigidCreated)
			return success;

		//try to move to the child shapes, otherwise continue on
		if(!reader.MoveToChildNode())
			return success;

		do 
		{
			currName = reader.GetCurrentAttributeName();

			if(currName == ShapeName)
			{
				std::string shapeName = reader.GetStringValue(true);

				if(!rigid->AttachShape(shapeName))
					success = false;
			}

		} while (reader.MoveToNextSiblingNode());

		//pop off shapes node
		reader.PopNode();
		//rigid->CreateDebugDraw();
		return success;
	}

	return false;	
}

bool SceneSerializer::DeserializeParticleComponent(XMLReader& reader, SharedGameObject& objectToAdd)
{
	if(reader.NodeHasAttributes())
	{
		std::shared_ptr<AbstractParticleEmitter> emitter;
		std::vector<std::shared_ptr<ParticleAffector>> affectorList;

		bool emitterLoaded = false;
		bool affectorsLoaded = false;

		if(reader.MoveToChildNode())
		{
			std::string currNodeName;
			do 
			{
				currNodeName = reader.GetCurrentNodeName();

				if(currNodeName == ParticleAffectors)
					affectorsLoaded = DeserializeParticleAffectors(reader, affectorList);
				else if(currNodeName.find(Emitter))
					emitterLoaded = DeserializeParticleEmitter(reader, emitter);
				
			} while (reader.MoveToNextSiblingNode());
			reader.PopNode();
		}
		else
			return false;

		bool enabled, particleSpace, onGPU;
		long long particleID, maxParticles;
		float initialLifetime;
		physx::PxParticleSystem* particleBase = NULL;

		bool currResult = true;
		bool failureDetected = false;

		std::string currAttr = reader.GetCurrentNodeName();
		do 
		{
			currAttr = reader.GetCurrentAttributeName();

			if(currAttr == Enabled)
				currResult = reader.GetBoolValue(enabled, true);
			else if(currAttr == ParticleSpace)
				currResult = reader.GetBoolValue(particleSpace, true);
			else if(currAttr == ParticleActorID)
			{
				currResult = reader.GetLongValue(particleID, true);

				if(currResult)
				{
					physx::PxBase* actor = PhysXSerializerWrapper::FindByID(ActorCollection, particleID);
					if(actor)
					{
						if(actor->getConcreteType() == physx::PxConcreteType::ePARTICLE_SYSTEM)
							particleBase = (physx::PxParticleSystem*)actor;
					}
				}
			}
			else if(currAttr == MaxParticles)
				currResult = reader.GetLongValue(maxParticles, true);
			else if(currAttr == OnGPU)
				currResult = reader.GetBoolValue(onGPU, true);
			else if(currAttr == InitialLifetime)
				currResult = reader.GetFloatValue(initialLifetime, true);

			if(!currResult)
				failureDetected = true;

		} while (reader.MoveToNextAttribute());

		if(!failureDetected)
		{
			ParticleSystemBase* particleSystem = new ParticleSystemBase(particleBase, emitter,
				maxParticles, initialLifetime);
			
			ParticleComponent* particleComponent = new ParticleComponent(particleSystem, particleSpace);

			objectToAdd->AttachComponent(particleComponent);

			for (int i = 0; i < affectorList.size(); ++i)
			{
				particleSystem->AddAffector(affectorList[i]);
			}
			if(onGPU)
			{
				physx::PxCudaContextManager* cudaMan = objectToAdd->GetOwningScene()->GetCudaContextManager();
				if(cudaMan)
				{
					particleSystem->onGPU = onGPU;
					particleSystem->SetPhysXCudaManager(cudaMan);

					particleSystem->AssignAffectorKernel(particleSystem->FindBestKernel());
				}
			}

			particleSystem->GetMaterial()->CreateMaterial(particleSystem);
			particleSystem->setMaterial(particleSystem->GetMaterial()->GetMaterialName());

			return true;
		}
	}

	return false;
}

bool SceneSerializer::DeserializeParticleEmitter(XMLReader& reader, 
		std::shared_ptr<AbstractParticleEmitter>& outEmitter)
{
	//<PointEmitter ParticlesPerSecond="5000.000000" MinSpeed="10.000000" MaxSpeed="20.000000" Duration="2.000000">
	//				<Position X="0.000000" Y="0.000000" Z="0.000000"/>
	//				<MinDirection X="-0.707107" Y="-0.707107" Z="0.000000"/>
	//				<MaxDirection X="0.707107" Y="0.707107" Z="0.000000"/>
	//		</PointEmitter>
	
	bool failureDetected = false;
	bool currentResult = true;

	if(reader.NodeHasAttributes())
	{
		std::string currAttr;

		float pps, minSpeed, maxSpeed, duration;
		physx::PxVec3 position, minDirection, maxDirection;
		bool emitLoop;

		do 
		{
			currAttr = reader.GetCurrentAttributeName();

			if(currAttr == PPS)
				currentResult = reader.GetFloatValue(pps, true);
			else if(currAttr == MinEmitSpeed)
				currentResult = reader.GetFloatValue(minSpeed, true);
			else if(currAttr == MaxEmitSpeed)
				currentResult = reader.GetFloatValue(maxSpeed, true);
			else if(currAttr == Duration)
				currentResult = reader.GetFloatValue(duration, true);
			else if(currAttr == EmitLoop)
				currentResult = reader.GetBoolValue(emitLoop, true);

			if(!currentResult)
				failureDetected = true;

		} while (reader.MoveToNextAttribute());

		if(reader.MoveToChildNode())
		{
			std::string currNodeName;

			do 
			{
				currNodeName = reader.GetCurrentNodeName();

				if(currNodeName == Position)
					currentResult = DeserializeVector3(reader, position);
				else if(currNodeName == MinEmitDirection)
					currentResult = DeserializeVector3(reader, minDirection);
				else if(currNodeName == MaxEmitDirection)
					currentResult = DeserializeVector3(reader, maxDirection);

				if(!currentResult)
					failureDetected = true;
				
			} while (reader.MoveToNextSiblingNode());
			//Move back up a level
			reader.PopNode();
		}

		std::string nodeName = reader.GetCurrentNodeName();

		bool emitterCreated = false;
		outEmitter = nullptr;

		if (nodeName == PointEmitter)
		{
			outEmitter = std::make_shared<ParticlePointEmitter>(ParticlePointEmitter(pps, position, 
				minDirection, maxDirection, duration, minSpeed, maxSpeed));
			emitterCreated = true;
		}
	}

	//return true if no failure detected and out emitter was assigned
	return (!failureDetected && outEmitter);
}

bool SceneSerializer::DeserializeParticleAffectors(XMLReader& reader, 
			std::vector<std::shared_ptr<ParticleAffector>>& outList)
{
	bool failureDetected = false;
	bool currResult = false;

	if(reader.MoveToChildNode())
	{
		std::string currNodeName; 
		std::string currAttName; 

		do 
		{
			currNodeName= reader.GetCurrentNodeName();

			if (currNodeName == ScaleAffector)
			{
				std::shared_ptr<ScaleParticleAffector> scaleAffector;
				currResult = DeserializeScaleAffector(reader, scaleAffector);
				if(currResult)
					outList.push_back(scaleAffector);
				else
					failureDetected = true;				
			}
			else if(currNodeName == ColourFaderAffector)
			{
				std::shared_ptr<ColourFadeParticleAffector> colorAffector;
				currResult = DeserializeColourFadeAffector(reader, colorAffector);
				if(currResult)
					outList.push_back(colorAffector);
				else
					failureDetected = true;
			}

		} while (reader.MoveToNextSiblingNode());

		reader.PopNode();

		return !failureDetected;
	}

	return false;
}

bool SceneSerializer::DeserializeScaleAffector(XMLReader& reader, std::shared_ptr<ScaleParticleAffector> &affector)
{
	bool currResult = true;
	bool enlarge, onGPU = false, scaleSuccess = true;
	float minScale, maxScale;

	std::string currAttName;

	do 
	{
		currAttName = reader.GetCurrentAttributeName();
		if(currAttName == OnGPU)
			currResult = reader.GetBoolValue(onGPU, true);
		else if(currAttName == Enlarge)
			currResult = reader.GetBoolValue(enlarge, true);
		else if(currAttName == MinScale)
			currResult = reader.GetFloatValue(minScale, true);
		else if(currAttName == MaxScale)
			currResult = reader.GetFloatValue(maxScale, true);

		if(!currResult)
			scaleSuccess = false;
	} while (reader.MoveToNextAttribute());
	
	if(scaleSuccess)
		affector = std::make_shared<ScaleParticleAffector>(enlarge, minScale, maxScale, onGPU);

	return scaleSuccess;
}

bool SceneSerializer::DeserializeColourFadeAffector(XMLReader& reader, std::shared_ptr<ColourFadeParticleAffector> &affector)
{
	if(reader.MoveToChildNode())
	{
		std::string currNodeName;
		physx::PxVec4 startColour, endColour;
		bool onGPU = false, success = true;

		do 
		{
			currNodeName = reader.GetCurrentNodeName();

			if(currNodeName == StartColour)
				if(!DeserializeVector4(reader, startColour))
					success = false;

			if(currNodeName == EndColour)
				if(!DeserializeVector4(reader, endColour))
					success = false;

		} while (reader.MoveToNextSiblingNode());

		reader.PopNode();

		if(reader.NodeHasAttributes())
		{
			if(reader.GetCurrentAttributeName() == OnGPU)
				success = reader.GetBoolValue(onGPU, true);
		}

		if(success)
			affector = std::make_shared<ColourFadeParticleAffector>(startColour, endColour, onGPU);

		return success;
	}

	return false;
}


