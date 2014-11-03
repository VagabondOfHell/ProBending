#pragma once
#include "RenderSystems/GL/OgreGLHardwareVertexBuffer.h"
#include "PxPhysicsAPI.h"
#include "cuda.h"

class IScene;

class CudaInteropBuffer
{
private:
	Ogre::HardwareVertexBufferSharedPtr mOgreBuffer;

	CUgraphicsResource cudaBuffer;

	IScene* owningScene;

	size_t bufferSize;
	CUdeviceptr cudaPointer;

	bool isMapped;
	bool isRegistered;

public:
	///<summary>Constructor. Does not register the buffer automatically, as it is not provided </summary>
	///<param "owningScene">The owning scene that holds the Cuda context</param>
	CudaInteropBuffer(IScene* owningScene);

	///<summary>Constructor. Also registers the buffer </summary>
	///<param "owningScene">The owning scene that holds the Cuda context</param>
	///<param "buffer">The Ogre Buffer that will be registered</param>
	CudaInteropBuffer(IScene* owningScene, Ogre::HardwareVertexBufferSharedPtr buffer);

	///<summary>Destructor. Unmaps and Unregisters as required</summary>
	~CudaInteropBuffer(void);
	
	///<summary>Sets the Ogre Vertex Buffer. Does not Unregister or Unmap previous. 
	///This must be done by the client</summary>
	inline void SetOgreBuffer(Ogre::HardwareVertexBufferSharedPtr buffer)
	{
		mOgreBuffer = buffer;
	}

	///<summary>Registers the buffer with a Cuda Resource, allowing CUDA to share memory with GL</summary>
	///<returns>True if registered, false if not</returns>
	bool RegisterBufferToCuda();

	///<summary>Unregisters the buffer with a Cuda Resource, allowing Cuda to clean up</summary>
	///<returns>True if registered, false if not</returns>
	bool UnregisterBufferFromCuda();
	
	///<summary>Calls the Copy Kernel that PhysX provides</summary>
	///<param "source">The device to copy the data from</param>
	///<returns>True if registered, false if not</returns>
	void LaunchPhysxCopyKernel(CUdeviceptr source);

	///<summary>Maps the Cuda Graphics Resource for buffer manipulation</summary>
	///<returns>The CUDA result status</returns>
	inline CUresult MapToCudaBuffer()
	{
		//If not already mapped
		if(!isMapped)
		{
			//Map it
			CUresult result = cuGraphicsMapResources(1, &cudaBuffer, 0);
			
			//If successful, flag IsMapped
			if(result == CUDA_SUCCESS)
				isMapped = true;

			//Return result
			return result;
		}
		else
			//Otherwise, return already mapped
			return CUDA_ERROR_ALREADY_MAPPED;
	}

	///<summary>Unmaps the Cuda Graphics Resource to mark the end of buffer manipulation</summary>
	///<returns>The CUDA result status</returns>
	inline CUresult UnmapFromCudaBuffer()
	{
		//If it has been mapped already
		if(isMapped)
		{
			//Try to unmap it
			CUresult result = cuGraphicsUnmapResources(1, &cudaBuffer, 0);

			//Check for success and flag the IsMapped variable
			if(result == CUDA_SUCCESS)
				isMapped = false;

			//Return results
			return result;
		}
		else
			//If not already mapped, return the error
			return CUDA_ERROR_NOT_MAPPED;
	}

	///<summary>Gets the device pointer to the Graphics Resource in GPU memory</summary>
	///<param "device">The CUDeviceptr to store the Device Result in</param>
	///<param "bufferSize">The size_t to store the buffer size in</param>
	///<returns>The CUDA result status</returns>
	inline CUresult GetCudaGraphicsMemoryPointer()
	{
		//If it isn't mapped, return the proper error
		if(!isMapped)
			return CUDA_ERROR_NOT_MAPPED;

		//Get the memory pointer
		return cuGraphicsResourceGetMappedPointer_v2(&cudaPointer, &bufferSize, cudaBuffer);	
	}
	
	///<summary>Maps the Cuda Buffer and then gets the device pointer to the Graphics Resource in GPU memory</summary>
	///<param "device">The CUDeviceptr to store the Device Result in</param>
	///<param "bufferSize">The size_t to store the buffer size in</param>
	///<returns>The CUDA result status</returns>
	inline CUresult MapAndGetCudaGraphicsMemoryPointer()
	{
		//If it isn't mapped, map it
		if(!isMapped)
			MapToCudaBuffer();

		//Get the device pointer to the mapped resource
		return cuGraphicsResourceGetMappedPointer_v2(&cudaPointer, &bufferSize, cudaBuffer);	
	}

	///<summary>Gets whether or not the CUDA resource is currently mapped</summary>
	///<returns>True if mapped, false if not</returns>
	inline bool GetIsMapped()
	{
		return isMapped;
	}

	///<summary>Gets whether or not the CUDA resource has been registered</summary>
	///<returns>True if registered, false if not</returns>
	inline bool GetIsRegistered()
	{
		return isRegistered;
	}

	///<summary>Gets the GL Buffer address</summary>
	///<returns>GLuint representing the address of the buffer</returns>
	inline GLuint GetGLBufferID()
	{
		Ogre::GLHardwareVertexBuffer* bufferGL = static_cast<Ogre::GLHardwareVertexBuffer*>(mOgreBuffer.getPointer());
		return bufferGL->getGLBufferId();
	}

	///<summary>Gets the Cuda Device Pointer. Should only be retrieved while mapped, or shortly after</summary>
	///<returns>The Cuda Device Pointer of this buffer</returns>
	inline const CUdeviceptr GetCudaDevicePointer()
	{
		return cudaPointer;
	}

	///<summary>Gets the size of the Buffer of the Cuda Device Pointer. Should only be retrieved while mapped, or shortly after</summary>
	///<returns>The size of this buffer</returns>
	inline const size_t GetBufferSize()
	{
		return bufferSize;
	}

	///<summary>Gets the Ogre GL Buffer</summary>
	///<returns>The Ogre GL Vertex Hardware Buffer</returns>
	inline Ogre::HardwareVertexBufferSharedPtr GetOgreBuffer()
	{
		return mOgreBuffer;
	}
};

