#include "ParticleSystemMaterial.h"

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
	affectorFlags = 0;
}

ParticleSystemMaterial::~ParticleSystemMaterial(void)
{
}

std::string ParticleSystemMaterial::CreateMaterial(const std::string& matName)
{
	std::string nameToUse = matName;

	if(matName.empty())
		nameToUse = gen.generate();
	
	Ogre::MaterialPtr newMat = Ogre::MaterialManager::getSingletonPtr()->
		create(nameToUse, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, false);

	Ogre::Pass* newMatPass = newMat->getTechnique(0)->getPass(0);

	newMatPass->setLightingEnabled(false);
	newMatPass->setDepthWriteEnabled(false);
	newMatPass->setSceneBlending(Ogre::SBF_ONE, Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
	newMatPass->setPointAttenuation(true);
	newMatPass->setPointSpritesEnabled(true);	
	newMatPass->setPointSize(1.0f);
	
	std::string preProcessParams = GetPreprocessorParameters();

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

	newMatPass->setVertexProgramParameters(vertexParameters);

	Ogre::GpuProgramParametersSharedPtr fragParameters = fragment->createParameters();
	Ogre::GpuNamedConstants h = fragParameters->getConstantDefinitions();
	fragParameters->setNamedConstant("tex", (int)0);

	newMatPass->setFragmentProgramParameters(fragParameters);

	material = newMat;

	return nameToUse;
}

std::string ParticleSystemMaterial::GetPreprocessorParameters()
{
	if(affectorFlags == ParticleAffectorType::None)
		return "";

	std::string retVal;
	std::string comma = ",";
	std::string eq1 = "=1";
	std::string eq0 = "=0";

	if(affectorFlags & ParticleAffectorType::Scale)
		retVal = SCALE_PPD + eq1 + comma;
	else
		retVal = SCALE_PPD + eq0 + comma;

	if(affectorFlags & ParticleAffectorType::ColourToColour)
		retVal += COLOUR_PPD + eq1 + comma;
	else
		retVal += COLOUR_PPD + eq0 + comma;

	if(affectorFlags & ParticleAffectorType::Texture)
		retVal += TEXTURE_PPD + eq1;
	else
		retVal += TEXTURE_PPD + eq0;

	return retVal;
}

void ParticleSystemMaterial::Recompile()
{
	std::string preprocessorParams = GetPreprocessorParameters();
	
	material->getTechnique(0)->getPass(0)->getVertexProgram()->setParameter(OGRE_PPD, preprocessorParams);
	material->getTechnique(0)->getPass(0)->getFragmentProgram()->setParameter(OGRE_PPD, preprocessorParams);

	material->getTechnique(0)->getPass(0)->getVertexProgram()->reload();
	material->getTechnique(0)->getPass(0)->getFragmentProgram()->reload();
}

bool ParticleSystemMaterial::AddTextureUnit(const std::string& texName)
{
	Ogre::Pass* pass = material->getTechnique(0)->getPass(0);

	Ogre::TextureUnitState* state = new Ogre::TextureUnitState(pass, "smoke.png");
	state->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	pass->addTextureUnitState(state);
	return true;
}

bool ParticleSystemMaterial::ModifyTextureUnit(const unsigned short texUnitIndex, const std::string& texName)
{
	Ogre::TextureUnitState* unitState = material->getTechnique(0)->getPass(0)->getTextureUnitState(texUnitIndex);
	if(unitState)
		unitState->setTextureName(texName);

	return unitState != NULL;
}
