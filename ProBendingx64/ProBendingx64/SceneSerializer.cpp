#include "SceneSerializer.h"

#include "PhysXDataManager.h"
#include "PhysXSerializerWrapper.h"

#include "NotImplementedException.h"

#include "IScene.h"

#include "Component.h"
#include "MeshRenderComponent.h"
#include "RigidBodyComponent.h"
#include "ParticleComponent.h"

#include "PxRigidDynamic.h"
#include "PxRigidStatic.h"

#include "ParticleSystemBase.h"
#include "ParticlePointEmitter.h"
#include "ColourFadeParticleAffector.h"
#include "ParticleAffectors.h"
#include "ParticleAffectorEnum.h"

typedef std::unordered_set<SharedGameObject> GameObjectList;

SceneSerializer::SceneSerializer()
	: StartID(1), ActorCollection("ActorCollection"), PxDataCollection("PxDataCollection"),
	GameObjectNode("GameObjectNode"), ObjectName("ObjectName"),
	Position("Position"), X("X"), Y("Y"), Z("Z"), W("W"), Rotation("Rotation"),
	MeshRenderComponen("MeshRenderComponent"), RigidBodyComponen("RigidBodyComponent"),
	ParticleComponen("ParticleComponent"), EntityName("Entity"), RigidID("RigidID"),
	ParticleSpace("ParticleSpace"), PointEmitter("PointEmitter"), LineEmitter("LineEmitter"), 
	MeshEmitter("MeshEmitter"), MinEmitDirection("MinDirection"), MaxEmitDirection("MaxDirection"), 
	PPS("ParticlesPerSecond"), MinEmitSpeed("MinSpeed"), MaxEmitSpeed("MaxSpeed"), Duration("Duration"),
	ParticleActorID("ParticleActorID"), MaxParticles("MaxParticles"), OnGPU("OnGPU"), 
	InitialLifetime("InitialLifetime"), ParticleAffectors("ParticleAffectors"), ScaleAffector("ScaleAffector"), 
	ColourFaderAffector("ColourFadeAffector"), AlphaFadeAffector("AlphaFadeAffector"), Enlarge("Enlarge"), 
	MinScale("MinScale"), MaxScale("MaxScale"), StartColour("StartColour"), EndColour("EndColour")
{

}

SceneSerializer::~SceneSerializer()
{

}

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
bool SceneSerializer::SerializeScene(const IScene* scene, const std::string& fileName)
{
	PxDataManSerializeOptions serialOptions = PxDataManSerializeOptions(PxDataManSerializeOptions::ALL,
		PxDataCollection, true, fileName);

	if(PhysXDataManager::GetSingletonPtr()->SerializeData(serialOptions))
	{
		XMLWriter writer;
		std::string sceneName = scene->GetSceneName();

		if(PhysXSerializerWrapper::CreateSerializer() &&
			PhysXSerializerWrapper::CreateCollection(ActorCollection, true))
		{
			writer.CreateNode(sceneName, std::string(""), false);//Root node which is scene name

			for (GameObjectList::iterator start = scene->gameObjectList.begin();
				start != scene->gameObjectList.end(); ++start)
			{
				SerializeGameObject(writer, *start);//still need to handle children
			}

			PhysXSerializerWrapper::CompleteCollection(ActorCollection, PxDataCollection);

			bool serialize = PhysXSerializerWrapper::SerializeToBinary(fileName + "Actors.pbd", ActorCollection);
			bool xml = writer.WriteFile(fileName + ".xml");

			PhysXSerializerWrapper::ReleaseCollection(PxDataCollection);
			PhysXSerializerWrapper::ReleaseCollection(ActorCollection);
			PhysXSerializerWrapper::DestroySerializer();

			return serialize && xml;
		}		
	}

	return false;
}

void SceneSerializer::SerializeGameObject(XMLWriter& writer, const SharedGameObject gameObject)
{
	Ogre::Vector3 position = gameObject->GetLocalPosition();
	Ogre::Vector3 scale = gameObject->GetLocalScale();
	Ogre::Quaternion rotation = gameObject->GetLocalOrientation();

	writer.CreateNode(GameObjectNode, std::string(""), true); writer.AddAttribute(ObjectName, gameObject->GetName());
		//game object Transform
		writer.CreateNode(Position);
		writer.AddAttribute(X, position.x); writer.AddAttribute(Y, position.y); writer.AddAttribute(Z, position.z);
		writer.PopNode();

		writer.CreateNode(Rotation);
		writer.AddAttribute(X, rotation.x);writer.AddAttribute(Y, rotation.y);writer.AddAttribute(Z, rotation.z);
			writer.AddAttribute(W,rotation.w);
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

	writer.PopNode();
}

bool SceneSerializer::SerializeMeshRenderComponent(XMLWriter& writer, const MeshRenderComponent* renderer)
{
	//create render component node, setting enabled as its value
	if(writer.CreateNode(MeshRenderComponen, renderer->enabled, true))
	{
		writer.AddAttribute(EntityName, renderer->GetMeshName(), false, false);
		writer.PopNode();
		return true;
	}

	return false;
}

bool SceneSerializer::SerializeRigidBodyComponent(XMLWriter& writer, const RigidBodyComponent* rigidBody)
{
	RigidBodyComponent::RigidBodyType rigidType = rigidBody->GetBodyType();

	if(rigidType == RigidBodyComponent::NONE)
		return false;

	if(writer.CreateNode(RigidBodyComponen, rigidBody->enabled, true))
	{
		physx::PxActor* actor = NULL;

		if(rigidBody->GetBodyType() == RigidBodyComponent::DYNAMIC)
			actor = rigidBody->GetDynamicActor();
		else if(rigidBody->GetBodyType() == RigidBodyComponent::STATIC)
			actor = rigidBody->GetStaticActor();

		if(actor)
		{
			PhysXSerializerWrapper::AddToWorkingCollection(*actor);
			PhysXSerializerWrapper::CreateIDs(StartID);

			long long actorID = PhysXSerializerWrapper::GetID(ActorCollection, *actor);

			if(actorID > PX_SERIAL_OBJECT_ID_INVALID)
				writer.AddAttribute(RigidID, actorID);
		}

		writer.PopNode();

		return true;
	}

	return false;
}

bool SceneSerializer::SerializeParticleComponent(XMLWriter& writer, const ParticleComponent* particles)
{
	if(writer.CreateNode(ParticleComponen, particles->IsEnabled(), true))
	{
		writer.AddAttribute(ParticleSpace, particles->GetTransformationSpace(), false);//Store transformation space
		
		ParticleSystemBase* particleSystem = particles->particleSystem;

		//Add particle system actor to the collection and store the ID in our format
		if(PhysXSerializerWrapper::AddToWorkingCollection(*particleSystem->pxParticleSystem))
		{
			PhysXSerializerWrapper::CreateIDs(ActorCollection, StartID);
			long long systemID = PhysXSerializerWrapper::GetID(ActorCollection, 
				*particleSystem->pxParticleSystem);

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
	ParticleSystemBase* particleSystem = particles->particleSystem;

	if(particleSystem->affectorMap.size() > 0)
		writer.CreateNode(ParticleAffectors);
	else
		return true;//if none to process, return as if successful

	for (auto affectIter = particleSystem->affectorMap.begin();
		affectIter != particleSystem->affectorMap.end(); ++affectIter)
	{
		ParticleAffectorType::ParticleAffectorType affectType = 
			affectIter->second->GetAffectorType();

		switch (affectType)
		{
		case ParticleAffectorType::Scale:
			{
				if(writer.CreateNode(ScaleAffector, affectIter->second->GetOnGPU()))
				{
					GPUScaleAffectorParams* params = 
						(GPUScaleAffectorParams*)affectIter->second->GetGPUParamaters();

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
				if(writer.CreateNode(ColourFaderAffector, affectIter->second->GetOnGPU()))
				{
					GPUColourFaderAffectorParams* params =
						(GPUColourFaderAffectorParams*)affectIter->second->GetGPUParamaters();

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

bool SceneSerializer::DeserializeScene(const IScene* scene, const std::string& fileName)
{
	PxDataManSerializeOptions serialOptions = PxDataManSerializeOptions(PxDataManSerializeOptions::ALL,
		"SerializationCollection", true, fileName);

	throw NotImplementedException();

	return false;

}


