#include "CudaInteropBuffer.h"
#include "IScene.h"

CudaInteropBuffer::CudaInteropBuffer(IScene* _owningScene)
{
	owningScene = _owningScene;

	isRegistered = false;
	isMapped = false;
}

CudaInteropBuffer::CudaInteropBuffer(IScene* _owningScene, Ogre::HardwareVertexBufferSharedPtr buffer)
{
	owningScene = _owningScene;
	mOgreBuffer = buffer;

	isRegistered = false;
	isMapped = false;

	RegisterBufferToCuda();
}

CudaInteropBuffer::~CudaInteropBuffer(void)
{
	//Unmap the buffer if its mapped
	UnmapFromCudaBuffer();

	//Unregister the buffer if its registered
	UnregisterBufferFromCuda();	
}

bool CudaInteropBuffer::RegisterBufferToCuda()
{
	//If it isn't already registered
	if(!isRegistered)
	{
		//Get the physx Cuda context to be allowed to use it
		owningScene->GetCudaContextManager()->acquireContext();

		//Tie the GL Buffer to the CUDA buffer
		bool result = owningScene->GetCudaContextManager()->registerResourceInCudaGL(cudaBuffer, GetGLBufferID());

		if(result)
			isRegistered = true;

		//Allow the context to be used elsewhere
		owningScene->GetCudaContextManager()->releaseContext();

		//return results
		return result;
	}

	return false;
}

bool CudaInteropBuffer::UnregisterBufferFromCuda()
{
	//If it has been registered
	if(isRegistered)
	{
		//Acquire the GPU Context
		owningScene->GetCudaContextManager()->acquireContext();

		//Get the result of unregistering the resource
		bool result = owningScene->GetCudaContextManager()->unregisterResourceInCuda(cudaBuffer);

		//If successful, set registration flag
		if(result)
			isRegistered = false;

		//Release the GPU Context to be able to use it elsewhere
		owningScene->GetCudaContextManager()->releaseContext();

		return result;
	}
	else
		return false;
}

void CudaInteropBuffer::LaunchPhysxCopyKernel(CUdeviceptr source)
{
	physx::PxGpuCopyDesc copyData;
	copyData.bytes = bufferSize;
	copyData.type = copyData.DeviceToDevice;
	copyData.source = source;
	copyData.dest = cudaPointer;

	owningScene->GetCudaContextManager()->getGpuDispatcher()->launchCopyKernel(&copyData, 1, 0);
}