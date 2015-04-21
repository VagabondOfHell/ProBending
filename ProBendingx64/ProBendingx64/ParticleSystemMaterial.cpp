#include "ParticleSystemMaterial.h"

#include "ParticleSystemBase.h"

#include "OgreHighLevelGpuProgramManager.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"

const std::string ParticleSystemMaterial::TEXTURE_PPD = "USE_TEXTURE";
const std::string ParticleSystemMaterial::COLOUR_PPD = "PER_PARTICLE_COLOUR";
const std::string ParticleSystemMaterial::SCALE_PPD = "PER_PARTICLE_SCALE";
const std::string ParticleSystemMaterial::OGRE_PPD = "preprocessor_defines";

Ogre::NameGenerator ParticleSystemMaterial::gen = Ogre::NameGenerator("mat");

ParticleSystemMaterial::ParticleSystemMaterial(void)
{
}

ParticleSystemMaterial::~ParticleSystemMaterial(void)
{
}

std::string ParticleSystemMaterial::CreateMaterial(ParticleSystemBase* system,
		unsigned int textureSlots /*= 0*/, const std::string& matName)
{
	AffectorData* affectors = system->GetAffectorData();

	ParticleAffectorType::ParticleAffectorFlag affectorFlags = affectors->allTypesCombination;

	std::string nameToUse = matName;

	if(matName.empty())
		nameToUse = gen.generate();
	
	Ogre::MaterialPtr newMat = Ogre::MaterialManager::getSingletonPtr()->
		create(nameToUse, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);

	Ogre::Pass* newMatPass = newMat->getTechnique(0)->getPass(0);

	newMatPass->setLightingEnabled(false);
	newMatPass->setDepthWriteEnabled(false);
	newMatPass->setSceneBlending(Ogre::SBF_SOURCE_ALPHA, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
	newMatPass->setPointAttenuation(true);
	newMatPass->setPointSpritesEnabled(true);	
	newMatPass->setPointSize(1.0f);
	
	std::string preProcessParams = GetPreprocessorParameters(affectorFlags, textureSlots);

	std::string vertProgramName = nameToUse + "VertexProgram";
	Ogre::HighLevelGpuProgramPtr vertex = Ogre::HighLevelGpuProgramManager::getSingletonPtr()->createProgram(vertProgramName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsl", Ogre::GPT_VERTEX_PROGRAM);
	vertex->setSourceFile("ParticleShader.vert");
	vertex->setParameter(OGRE_PPD, preProcessParams);
	//vertex->setParameter(OGRE_PPD, "PER_PARTICLE_SCALE=1,PER_PARTICLE_COLOUR=1,USE_TEXTURE=1");
	vertex->load();

	std::string fragProgramName = nameToUse + "FragmentProgram";
	Ogre::HighLevelGpuProgramPtr fragment = Ogre::HighLevelGpuProgramManager::getSingletonPtr()->createProgram(fragProgramName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "glsl", Ogre::GPT_FRAGMENT_PROGRAM);
	fragment->setSourceFile("ParticleShader.frag");
	fragment->setParameter(OGRE_PPD, preProcessParams);
	//fragment->setParameter(OGRE_PPD, "PER_PARTICLE_SCALE=1,PER_PARTICLE_COLOUR=1,USE_TEXTURE=1");
	fragment->load();

	newMatPass->setVertexProgram(vertex->getName());
	newMatPass->setFragmentProgram(fragment->getName());

	Ogre::GpuProgramParametersSharedPtr vertexParameters = vertex->createParameters();

	vertexParameters->setNamedAutoConstant("worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX);
	vertexParameters->setNamedAutoConstant("camPos", Ogre::GpuProgramParameters::ACT_CAMERA_POSITION);
	
	Ogre::GpuProgramParametersSharedPtr fragParameters = fragment->createParameters();

	auto end = affectors->GetMapEnd();

	for (auto start = affectors->GetMapBegin(); start != end; ++start)
	{
		if(start->first == ParticleAffectorType::Texture)
		{
			const GPUTextureAffectorParams* textureParams = (GPUTextureAffectorParams*)start->second->GetParameters();
			unsigned int numTextures = textureParams->maxTextures;
			Ogre::Pass* pass = newMat->getTechnique(0)->getPass(0);

			/*fragParameters->setNamedConstant("tex0", (int)0);
			fragParameters->setNamedConstant("tex1", (int)1);*/
			for (unsigned int i = 0; i < textureSlots; i++)
			{
				fragParameters->setNamedConstant("tex" + std::to_string(i), (int)i);
			}
		}
	}

	newMatPass->setVertexProgramParameters(vertexParameters);
	newMatPass->setFragmentProgramParameters(fragParameters);

	material = newMat;

	return nameToUse;
}

std::string ParticleSystemMaterial::GetPreprocessorParameters(ParticleAffectorType::ParticleAffectorFlag affectorFlags, 
							unsigned int textureSlots /*= 0*/)
{
	if(affectorFlags == ParticleAffectorType::None)
		return "";

	std::string retVal;
	std::string comma = ",";
	std::string eq1 = "=1";
	std::string eq0 = "=0";
	std::string eqTexSlots = "=" + std::to_string(textureSlots);

	if(affectorFlags & ParticleAffectorType::Scale)
		retVal = SCALE_PPD + eq1 + comma;
	else
		retVal = SCALE_PPD + eq0 + comma;

	if(affectorFlags & ParticleAffectorType::ColourToColour)
		retVal += COLOUR_PPD + eq1 + comma;
	else
		retVal += COLOUR_PPD + eq0 + comma;

	if(affectorFlags & ParticleAffectorType::Texture)
		retVal += TEXTURE_PPD + eqTexSlots;
	else
		retVal += TEXTURE_PPD + eq0;

	return retVal;
}

void ParticleSystemMaterial::Recompile()
{
	/*std::string preprocessorParams = GetPreprocessorParameters();
	
	material->getTechnique(0)->getPass(0)->getVertexProgram()->setParameter(OGRE_PPD, preprocessorParams);
	material->getTechnique(0)->getPass(0)->getFragmentProgram()->setParameter(OGRE_PPD, preprocessorParams);

	material->getTechnique(0)->getPass(0)->getVertexProgram()->reload();
	material->getTechnique(0)->getPass(0)->getFragmentProgram()->reload();*/
}

bool ParticleSystemMaterial::AddTextureUnit(const std::string& texName)
{
	Ogre::Pass* pass = material->getTechnique(0)->getPass(0);

	Ogre::TextureUnitState* state = pass->createTextureUnitState(texName);
	state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	state->setTextureScale(0.05f, 0.05f);

	return true;
}

bool ParticleSystemMaterial::ModifyTextureUnit(const unsigned short texUnitIndex, const std::string& texName)
{
	Ogre::TextureUnitState* unitState = material->getTechnique(0)->getPass(0)->getTextureUnitState(texUnitIndex);
	if(unitState)
		unitState->setTextureName(texName);

	return unitState != NULL;
}

void ParticleSystemMaterial::GetShaderProgramParameters(AffectorData* affectors,
			Ogre::GpuProgramParameters& vertexParameters, Ogre::GpuProgramParameters& fragParameters)
{
	/*Ogre::Pass* pass = material->getTechnique(0)->getPass(0);

	Ogre::HighLevelGpuProgramPtr fragment = pass->getFragmentProgram();

	Ogre::GpuProgramParametersSharedPtr fragParams = fragment->createParameters();

	auto end = affectors->GetMapEnd();

	for (auto start = affectors->GetMapBegin(); start != end; ++start)
	{
	if(start->first == ParticleAffectorType::Texture)
	{
	GPUTextureAffectorParams* textureParams = (GPUTextureAffectorParams*)start->second->GetGPUParamaters();
	unsigned int numTextures = textureParams->maxTextures;
	Ogre::Pass* pass = material->getTechnique(0)->getPass(0);

	for (unsigned int i = 0; i < numTextures; i++)
	{
	fragParams->setNamedConstant("images[" + std::to_string(i) + "]", (int)i);
	Ogre::TextureUnitState* texState = pass->getTextureUnitState(i);
	if(texState)
	{
	fragParams->setNamedConstant("texture_matrices["+ std::to_string(i) + "]", texState->getTextureTransform());
	}

	}
	}
	}*/
}
