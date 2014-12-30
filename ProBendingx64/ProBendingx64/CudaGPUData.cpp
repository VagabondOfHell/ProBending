#include "CudaGPUData.h"


CudaGPUData::CudaGPUData(physx::PxCudaContextManager* _cudaContext, unsigned int numberOfGraphicsBuffers, unsigned int numberNonGraphicsBuffers)
{
	cudaContext = _cudaContext;
	graphicsResourceCount = numberOfGraphicsBuffers;
	nonGraphicsResourceCount = numberNonGraphicsBuffers;

	cudaGraphicsResources = new CUgraphicsResource[graphicsResourceCount];
	cudaNonGraphicsResources = new MappedGPUData[nonGraphicsResourceCount];

	graphicsRegisteredList = new bool[graphicsResourceCount];
	graphicsMappedList = new bool[graphicsResourceCount];

	for (unsigned int i = 0; i < graphicsResourceCount; ++i)
	{
		graphicsRegisteredList[i] = false;
		graphicsMappedList[i] = false;
	}
}


CudaGPUData::~CudaGPUData(void)
{
	if(cudaGraphicsResources)
	{
		delete cudaGraphicsResources;
		cudaGraphicsResources = NULL;
	}

	if(graphicsRegisteredList)
		delete[] graphicsRegisteredList;

	if(graphicsMappedList)
		delete[] graphicsMappedList;

	if(cudaNonGraphicsResources)
	{
		delete cudaNonGraphicsResources;
		cudaNonGraphicsResources = NULL;
	}
}

bool CudaGPUData::RegisterCudaGraphicsResource(unsigned int index, Ogre::HardwareVertexBufferSharedPtr ogreBuffer)
{
	//Validate index and check if already registered
	if(index >= graphicsResourceCount || graphicsRegisteredList[index])
		return false;

	cudaContext->acquireContext();

	//Attempt to register the resource
	bool result = cudaContext->registerResourceInCudaGL(cudaGraphicsResources[index], GetGLBufferID(ogreBuffer));

	cudaContext->releaseContext();

	if(result)
		graphicsRegisteredList[index] = true;

	return result;
}

bool CudaGPUData::UnregisterCudaGraphicsResource(unsigned int index)
{
	//Validate index and check if not registered
	if(index >= graphicsResourceCount || !graphicsRegisteredList[index])
		return false;

	cudaContext->acquireContext();

	//Attempt to unregister
	bool result = cudaContext->unregisterResourceInCuda(cudaGraphicsResources[index]);

	cudaContext->releaseContext();

	if(result)
		graphicsRegisteredList[index] = false;

	return result;
}

void CudaGPUData::UnregisterAllGraphicsResources()
{
	cudaContext->acquireContext();

	//Loop through and attempt to unregister each
	for (unsigned int i = 0; i < graphicsResourceCount; i++)
	{
		if(graphicsRegisteredList[i])
			graphicsRegisteredList[i] = cudaContext->unregisterResourceInCuda(cudaGraphicsResources[i]);
	}

	cudaContext->releaseContext();
}