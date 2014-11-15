#include "CudaGPUData.h"


CudaGPUData::CudaGPUData(physx::PxCudaContextManager* _cudaContext, unsigned int numberOfGraphicsBuffers, unsigned int numberNonGraphicsBuffers)
{
	cudaContext = _cudaContext;
	graphicsResourceCount = numberOfGraphicsBuffers;
	nonGraphicsResourceCount = numberNonGraphicsBuffers;

	cudaGraphicsResources = new CUgraphicsResource[graphicsResourceCount];
	cudaNonGraphicsResources = new MappedGPUData[nonGraphicsResourceCount];
}


CudaGPUData::~CudaGPUData(void)
{
	if(cudaGraphicsResources)
	{
		delete cudaGraphicsResources;
		cudaGraphicsResources = NULL;
	}

	if(cudaNonGraphicsResources)
	{
		delete cudaNonGraphicsResources;
		cudaNonGraphicsResources = NULL;
	}
}

bool CudaGPUData::RegisterCudaGraphicsResource(unsigned int index, Ogre::HardwareVertexBufferSharedPtr ogreBuffer)
{
	//Validate index
	if(index >= graphicsResourceCount)
		return false;

	cudaContext->acquireContext();

	//Attempt to register the resource
	bool result = cudaContext->registerResourceInCudaGL(cudaGraphicsResources[index], GetGLBufferID(ogreBuffer));

	cudaContext->releaseContext();

	return result;
}

bool CudaGPUData::UnregisterCudaGraphicsResource(unsigned int index)
{
	//Validate index provided
	if(index >= graphicsResourceCount)
		return false;

	cudaContext->acquireContext();

	//Attempt to unregister
	bool result = cudaContext->unregisterResourceInCuda(cudaGraphicsResources[index]);

	cudaContext->releaseContext();

	return result;
}

void CudaGPUData::UnregisterAllGraphicsResources()
{
	cudaContext->acquireContext();

	//Loop through and attempt to unregister each
	for (int i = 0; i < graphicsResourceCount; i++)
	{
		cudaContext->unregisterResourceInCuda(cudaGraphicsResources[i]);
	}

	cudaContext->releaseContext();
}