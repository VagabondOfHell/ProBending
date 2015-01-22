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

class SceneSerializer
{
private:
	const unsigned long long StartID;

	const std::string ActorCollection;
	const std::string GameObjectNode, ObjectName;
	const std::string Position, Rotation, X, Y, Z, W;
	const std::string MeshRenderComponen, RigidBodyComponen, ParticleComponen;
	const std::string EntityName, RigidID, ParticleSpace;
	const std::string PointEmitter, LineEmitter, MeshEmitter;
	const std::string MinEmitDirection, MaxEmitDirection, PPS, MinEmitSpeed, MaxEmitSpeed, Duration;
	const std::string ParticleActorID, MaxParticles, OnGPU, InitialLifetime;
	const std::string ParticleAffectors, ScaleAffector, ColourFaderAffector, AlphaFadeAffector;
	const std::string Enlarge, MinScale, MaxScale, StartColour, EndColour;

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

public:
	SceneSerializer();
	~SceneSerializer();

	bool SerializeScene(const IScene* scene, const std::string& fileName);

	bool DeserializeScene(const IScene* scene, const std::string& fileName);
};

