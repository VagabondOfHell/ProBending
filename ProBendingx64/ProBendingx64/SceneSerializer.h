#pragma once
#include "XMLWriter.h"
#include "XMLReader.h"

#include "foundation/PxVec3.h"
#include "foundation/PxVec4.h"

#include "OgreVector3.h"
#include "OgreVector4.h"

#include "GameObject.h"

#include <string>
#include <memory>

class IScene;
class MeshRenderComponent;
class RigidBodyComponent;
class ParticleComponent;

class AbstractParticleEmitter;
class ParticleAffector;
class ScaleParticleAffector;
class ColourFadeParticleAffector;

class SceneSerializer
{
private:
	long long StartID;

	const std::string ActorCollection, PxDataCollection;
	const std::string GameObjectNode, ObjectName;
	const std::string Position, Rotation, Scale, X, Y, Z, W;
	const std::string MeshRenderComponen, RigidBodyComponen, ParticleComponen, Enabled;
	const std::string EntityName, SubentityMaterial, Subentity, SubentityID, RigidID, ParticleSpace;
	const std::string Emitter, PointEmitter, LineEmitter, MeshEmitter;
	const std::string MinEmitDirection, MaxEmitDirection, PPS, MinEmitSpeed, MaxEmitSpeed, Duration, EmitLoop;
	const std::string ParticleActorID, MaxParticles, OnGPU, InitialLifetime;
	const std::string ParticleAffectors, ScaleAffector, ColourFaderAffector, AlphaFadeAffector;
	const std::string Enlarge, MinScale, MaxScale, StartColour, EndColour;

#pragma region Serialization

	bool AddVector3Attribute(XMLWriter& writer, const float x, const float y, const float z);

	bool AddVector3Attribute(XMLWriter& writer, physx::PxVec3 vec);

	bool AddVector3Attribute(XMLWriter& writer, Ogre::Vector3 vec);

	bool AddVector4Attribute(XMLWriter& writer, const float x, const float y, const float z, const float w);

	bool AddVector4Attribute(XMLWriter& writer, physx::PxVec4 vec);

	bool AddVector4Attribute(XMLWriter& writer, Ogre::Vector4 vec);

	void SerializeGameObject(XMLWriter& writer, const SharedGameObject gameObject);
	
	bool SerializeMeshRenderComponent(XMLWriter& writer, const MeshRenderComponent* renderer);

	bool SerializeRigidBodyComponent(XMLWriter& writer, const RigidBodyComponent* rigidBody);

	bool SerializeParticleComponent(XMLWriter& writer, const ParticleComponent* particles);

	bool SerializeParticleEmitter(XMLWriter& writer, const ParticleComponent* particles);

	bool SerializeParticleAffectors(XMLWriter& writer, const ParticleComponent* particles);

	bool SerializeAudioComponent(XMLWriter& writer);

#pragma endregion

	bool DeserializeVector3(XMLReader& reader, Ogre::Vector3& retVal);

	bool DeserializeVector3(XMLReader& reader, physx::PxVec3& retVal);

	bool DeserializeVector3(XMLReader& reader, float& outX, float& outY, float &outZ);

	bool DeserializeVector4(XMLReader& reader, Ogre::Vector4& retVal);

	bool DeserializeVector4(XMLReader& reader, physx::PxVec4& retVal);

	bool DeserializeVector4(XMLReader& reader, float& outX, float& outY, float &outZ, float &outW);

	SharedGameObject DeserializedGameObject(XMLReader& reader, IScene* const scene);

	bool DeserializeMeshRenderComponent(XMLReader& reader, SharedGameObject& objectToAdd);

	bool DeserializeRigidBodyComponent(XMLReader& reader, SharedGameObject& objectToAdd);

	bool DeserializeParticleComponent(XMLReader& reader, SharedGameObject& objectToAdd);

	///<summary>Deserializes a section corresponding to Particle Emitters</summary>
	///<param name="reader">The reader to perform processing with</param>
	///<param name="outEmitter">The variable to store the resultant emitter</param>
	///<returns>True if completely successful, false if an error occured. If false
	///a value is not set properly or emitter was not created. Must check for NULL</returns>
	bool DeserializeParticleEmitter(XMLReader& reader, std::shared_ptr<AbstractParticleEmitter>& outEmitter);

	bool DeserializeParticleAffectors(XMLReader& reader, std::vector<std::shared_ptr<ParticleAffector>>& outList);

	bool DeserializeScaleAffector(XMLReader& reader,  std::shared_ptr<ScaleParticleAffector> &affector);

	bool DeserializeColourFadeAffector(XMLReader& reader, std::shared_ptr<ColourFadeParticleAffector> &affector);

public:
	SceneSerializer();
	~SceneSerializer();

	bool SerializeScene(const IScene* scene, std::string rootNodeName, const std::string& fileName, 
		const std::string& refCollection = std::string(""));

	bool DeserializeScene(IScene* scene, const std::string& fileName,
		const std::string& refCollection = std::string(""), bool flushDataMan = false, bool flushScene = false);
};

