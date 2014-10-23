#include "ParticleRenderer.h"
#include "OgreHardwareBufferManager.h"
#include "RenderSystems/GL/OgreGLHardwareVertexBuffer.h"
#include "OgreRoot.h"
#include "PxPhysicsAPI.h"
#include "IScene.h"

ParticleRenderer::ParticleRenderer(IScene* _sceneOwner, UINT32 numParticles)
	:particleCount(numParticles), volumeSize(1024)
{
	owningScene = _sceneOwner;
}

ParticleRenderer::~ParticleRenderer(void)
{
}

void ParticleRenderer::CreateVertexBuffers()
{
	// our vertices are just points
	mRenderOp.operationType = Ogre::RenderOperation::OT_POINT_LIST;
	mRenderOp.useIndexes = false;//EBO
	
	mRenderOp.vertexData = new Ogre::VertexData();
	mRenderOp.vertexData->vertexCount = particleCount;
	mRenderOp.vertexData->vertexBufferBinding->unsetAllBindings();

	// POSITIONS
	// define the vertex format
	size_t currOffset = 0;
	mRenderOp.vertexData->vertexDeclaration->addElement(0, currOffset, Ogre::VET_FLOAT4, Ogre::VES_POSITION);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4);

	// allocate the vertex buffer
	mVertexBufferPosition = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
	mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
	mRenderOp.vertexData->vertexCount,
	Ogre::HardwareBuffer::HBU_DISCARDABLE,///Add dynamicWriteOnly as well?
	false);

	// bind positions to 0
	mRenderOp.vertexData->vertexBufferBinding->setBinding(0, mVertexBufferPosition);

	// COLORS
	// define the vertex format
	currOffset = 0;
	mRenderOp.vertexData->vertexDeclaration->addElement(1, currOffset, Ogre::VET_FLOAT4, Ogre::VES_DIFFUSE);
	currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT4);
	// allocate the color buffer
	mVertexBufferColor = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
	mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
	mRenderOp.vertexData->vertexCount,
	Ogre::HardwareBuffer::HBU_DISCARDABLE,///Add dynamicWriteOnly as well?
	false);

	// bind colors to 1
	mRenderOp.vertexData->vertexBufferBinding->setBinding(1, mVertexBufferColor);

	// Fill some random positions for particles
	Ogre::Vector4* pVertexPos = static_cast<Ogre::Vector4*>(mVertexBufferPosition->lock(Ogre::HardwareBuffer::HBL_NORMAL));
	
	for(UINT32 i = 0; i< particleCount ;i++)
	{
		Ogre::Vector3 pos = Ogre::Vector3(rand() % volumeSize, rand() % volumeSize, rand() % volumeSize);
		Ogre::Vector4 p = Ogre::Vector4(pos);
		pVertexPos[i] = p;
	}
	mVertexBufferPosition->unlock();

	// Fill some colors for particles
	Ogre::RGBA* pVertexColor = static_cast<Ogre::RGBA*>(mVertexBufferColor->lock(Ogre::HardwareBuffer::HBL_NORMAL));
	Ogre::RenderSystem* rs = Ogre::Root::getSingleton().getRenderSystem();

	for(UINT32 i = 0; i < particleCount; i++)
	{
		rs->convertColourValue(Ogre::ColourValue::Red, &pVertexColor[i]);
	}

	mVertexBufferColor->unlock();
}

void ParticleRenderer::MapToCudaBuffer()
{
	Ogre::GLHardwareVertexBuffer* bufferGL = static_cast<Ogre::GLHardwareVertexBuffer*>(mVertexBufferPosition.getPointer());
	GLuint glBufferID = bufferGL->getGLBufferId();

	
	//mSimCudaHelper->MapBuffer(devPtr, bufferGL_ID);
}