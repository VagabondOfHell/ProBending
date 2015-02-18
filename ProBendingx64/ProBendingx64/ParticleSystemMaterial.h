#pragma once
#include "ParticleAffectorEnum.h"

#include "OgreGpuProgramParams.h"
#include "OgreMaterial.h"
#include "OgreNameGenerator.h"

#include "AffectorData.h"

class ParticleSystemBase;

class ParticleSystemMaterial
{
protected:
	static const std::string SCALE_PPD;//Scale Affector Pre-processor definition
	static const std::string COLOUR_PPD;
	static const std::string TEXTURE_PPD;

	static const std::string OGRE_PPD;//Ogre Preprocessor Definition string

	static Ogre::NameGenerator gen;

	Ogre::MaterialPtr material;

	///<summary>Converts the set flags to their Preprocessor Directive Equivalents</summary>
	///<returns>string representing the parameters to pass to the material</returns>
	std::string GetPreprocessorParameters(ParticleAffectorType::ParticleAffectorFlag affectorFlags, unsigned int textureSlots = 0);

	void GetShaderProgramParameters(AffectorData* affectors,
		Ogre::GpuProgramParameters& vertexParameters, Ogre::GpuProgramParameters& fragParameters);

public:
	ParticleSystemMaterial(void);
	virtual ~ParticleSystemMaterial(void);

	///<summary>Creates the material to be used for the particle system</summary>
	///<param name="matName">Name that the material will be stored under in the Ogre::MaterialManager. Must be Unique. If
	///empty, a name is generated for you</param>
	///<returns>The name of the material that was created, or blank if failure</returns>
	std::string CreateMaterial(ParticleSystemBase* system, unsigned int textureSlots = 0, const std::string& matName = "");

	inline const std::string& GetMaterialName()const{return material->getName();}

	///<summary>Recompiles the shaders to use the appropriate data based on the attached affectors.
	///Should be called after affectors are added or removed</summary>
	void Recompile();

	bool AddTextureUnit(const std::string& texName);

	bool ModifyTextureUnit(const unsigned short texUnitIndex, const std::string& texName);
};

