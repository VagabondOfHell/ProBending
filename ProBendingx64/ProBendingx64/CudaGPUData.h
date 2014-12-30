#pragma once
#include "RenderSystems/GL/OgreGLHardwareVertexBuffer.h"
#include "cuda.h"
#include "pxtask\PxCudaContextManager.h"

struct MappedGPUData
{
	CUdeviceptr devicePointer; //The mapped device pointer
	size_t bufferSize; //The size of the CUDA buffer
	bool isValid;

	///<summary>Default constructor that creates an invalid MappedGPUData structure</summary>
	MappedGPUData()
		:devicePointer(NULL), bufferSize(0), isValid(false)
	{
		
	}

	///<summary>Constructor for a valid MappedGPUData</summary>
	///<param name="_devicePointer">The Cuda GPU device pointer</param>
	///<param name="_bufferSize">The size of the buffer allocated</param>
	///<param name="_isValid">True if the values contained are valid, false if not</param>
	///<returns>A MappedGPUData structure with the specified values</returns>
	MappedGPUData(CUdeviceptr _devicePointer, size_t _bufferSize, bool _isValid)
		:devicePointer(_devicePointer), bufferSize(_bufferSize), isValid(_isValid)
	{
		devicePointer = _devicePointer;
	}
};

class CudaGPUData
{
private:

	unsigned int graphicsResourceCount; //Amount of graphics resources (shared between OpenGL and CUDA)
	unsigned int nonGraphicsResourceCount; //Amount of non-graphics resources (shared between CPU and GPU)

	CUgraphicsResource* cudaGraphicsResources; //The collection of cuda graphics resources
	bool* graphicsRegisteredList;//Collection of bools to indicate if a resource has been registered
	bool* graphicsMappedList;//Collection of bools to indicate if a resource has been mapped

	MappedGPUData* cudaNonGraphicsResources; //The collection of non graphics resources

	physx::PxCudaContextManager* cudaContext; //The GPU context to use

	CUresult previousError; //The last error, for debugging

	///<summary>Gets the GL Buffer address</summary>
	///<returns>GLuint representing the address of the buffer</returns>
	inline GLuint GetGLBufferID(Ogre::HardwareVertexBufferSharedPtr ogreBuffer)const
	{
		Ogre::GLHardwareVertexBuffer* bufferGL = static_cast<Ogre::GLHardwareVertexBuffer*>(ogreBuffer.getPointer());
		return bufferGL->getGLBufferId();
	}

public:

	///<summary>The constructor of a manager over Cuda resources</summary>
	///<param name="_cudaContext">The cuda context as supplied by PhysX</param>
	///<param name="numberOfGraphicsBuffers">The number of buffers shared between OpenGL and PhysX</param>
	///<param name="numberOfNonGraphicsBuffers">The number of buffers shared between the CPU and GPU</param>
	CudaGPUData(physx::PxCudaContextManager* _cudaContext, unsigned int numberOfGraphicsBuffers, unsigned int numberNonGraphicsBuffers);

	~CudaGPUData(void);

#pragma region GraphicsResources

	inline bool GetIsRegistered(unsigned int index)
	{
		if(index < graphicsResourceCount)
			return graphicsRegisteredList[index];
		else
			return false;
	}
	///<summary>Registers an OpenGL buffer to the corresponding Cuda Graphics resource as specified by index</summary>
	///<param name="index">The index of the resource to register</param>
	///<param name="ogreBuffer">The GL buffer as provided by Ogre</param>
	///<returns>True if successful, false if not. If false, check Cuda Previous Error</returns>
	bool RegisterCudaGraphicsResource(unsigned int index, Ogre::HardwareVertexBufferSharedPtr ogreBuffer);

	///<summary>Unregisters a previously registered GL resource. Takes in the index of the cuda resource to remove</summary>
	///<param name="index">The index of the resource to unregister (Not the GL Buffer ID)</param>
	///<returns>True if successful, false if not. If false, check Cuda Previous Error</returns>
	bool UnregisterCudaGraphicsResource(unsigned int index);

	///<summary>Unregisters all previously registered GL resources. All resources must have been previously allocated
	///or this method will fail</summary>
	void UnregisterAllGraphicsResources();

	///<summary>Maps the Cuda Graphics Resource for buffer manipulation. All buffers must have been previously allocated
	///or this method will fail</summary>
	inline void MapAllResourcesToCuda()
	{
		for (unsigned int i = 0; i < graphicsResourceCount; i++)
		{
			MapResourceToCuda(i);
		}
	}

	///<summary>Maps a GL graphics resource to Cuda. Resource must have been previously allocated or 
	///this method will fail</summary>
	///<param name="index">The index of the resource to map</param>
	///<returns>True if successful, false if not. If false, check previous error</returns>
	inline bool MapResourceToCuda(unsigned int index)
	{
		//Validate index and check if already mapped
		if(index >= graphicsResourceCount || graphicsMappedList[index])
			return false;

		//Get status
		previousError = cuGraphicsMapResources(1, &cudaGraphicsResources[index], 0);

		graphicsMappedList[index] = (previousError == CUDA_SUCCESS);

		//Return result
		return previousError == CUDA_SUCCESS;
	}

	///<summary>Unmaps all gl resources from Cuda to allow GL to use it. Resource must have been previously allocated
	///or this method will fail</summary>
	inline void UnmapAllResourcesFromCuda()
	{
		for (unsigned int i = 0; i < graphicsResourceCount; i++)
		{
			UnmapResourceFromCuda(i);
		}
	}

	///<summary>Unmaps gl resource from Cuda to allow GL to use it. Resource must have been previously allocated
	///or this method will fail</summary>
	///<returns>True if successful, false if not. If false, check previous errors</returns>
	inline bool UnmapResourceFromCuda(unsigned int index)
	{
		//Validate index and make sure its already mapped
		if(index >= graphicsResourceCount || !graphicsMappedList[index])
			return false;

		//Unmap the one resource
		previousError = cuGraphicsUnmapResources(1, &cudaGraphicsResources[index], 0);

		graphicsMappedList[index] = !(previousError == CUDA_SUCCESS);

		return previousError == CUDA_SUCCESS;
	}

	///<summary>Get the physx cuda context</summary>
	///<returns>The physX Cuda Context Manager pointer</returns>
	inline physx::PxCudaContextManager* GetCudaContextManager()const
	{
		return cudaContext;
	}

	///<summary>Gets the mapped pointer device from CUDA. Resource must be mapped before calling this function</summary>
	///<param name = "index">The index to access</summary>
	///<returns>A structure representing the mapped pointer and size of the pointer, 
	///as well as the validity of the stucture. If structure is invalid, there was an error</returns>
	inline MappedGPUData GetGraphicsGPUDataPointer(unsigned int index)
	{
		//If we dont have valid index or its not mapped, return invalid data
		if(index >= graphicsResourceCount || !graphicsMappedList[index])
			return MappedGPUData(NULL, 0, false);

		CUdeviceptr devPointer;
		size_t size;

		//Get the data
		previousError = cuGraphicsResourceGetMappedPointer_v2(&devPointer, &size, cudaGraphicsResources[index]);

		//Return the structure
		return MappedGPUData(devPointer, size, previousError == CUDA_SUCCESS);
	}

	///<summary>Gets the mapped pointer device from CUDA for all graphics resources. All resources must
	///be previously mapped</summary>
	///<returns>A vector of structures representing the mapped pointer and size of the pointer, 
	///as well as the validity of the stucture. If a structure is invalid, there was an error.
	///Caller must delete the vector</returns>
	inline std::vector<MappedGPUData>* GetAllGraphicsGPUDataPointers()
	{
		//Create a pointer to a vector to prevent copying of excessive data
		std::vector<MappedGPUData>* mappedData = new std::vector<MappedGPUData>();
		//Reserve the number of resources in the vector that will be mapped
		mappedData->reserve(graphicsResourceCount);

		//loop through each resource
		for (unsigned int i = 0; i < graphicsResourceCount; i++)
		{
			//Get mapped pointers
			MappedGPUData data = GetGraphicsGPUDataPointer(i);

			if(data.isValid)
				//Add to the vector
				mappedData->push_back(data);
		}

		return mappedData;
	}
	
	///<summary>Maps the resource and gets the mapped pointer device from CUDA</summary>
	///<param name = "index">The index to access</param>
	///<returns>A structure representing the mapped pointer and size of the pointer, 
	///as well as the validity of the stucture. If structure is invalid, there was an error</returns>
	inline MappedGPUData MapAndGetGPUDataPointer(unsigned int index)
	{
		//If successfully mapped, get the pointer, otherwise return invalid results
		if(MapResourceToCuda(index))
			return GetGraphicsGPUDataPointer(index);
		else
			return MappedGPUData(NULL, 0, false);
	}

	///<summary>Maps and gets the mapped pointer device from CUDA for all graphics resources</summary>
	///<returns>A vector of structures representing the mapped pointer and size of the pointer, 
	///as well as the validity of the stucture. If a structure is invalid, there was an error for that instance.
	///Caller must delete the vector. Returns NULL if mapping failed</returns>
	inline std::vector<MappedGPUData>* MapAndGetAllGPUDataPointers()
	{
		MapAllResourcesToCuda();
		return GetAllGraphicsGPUDataPointers();
	}

#pragma endregion

#pragma region NonGraphicsResources

	///<summary>Helper method that wraps CUDA allocation in a static method. This data is not
	///managed by the CudaGPUData class. Client is responsible for any memory</summary>
	///<param name="retVal">Where to place the allocation results</param>
	///<param name="dataSizeInBytes">The amount of data in bytes that should be allocated</param>
	///<returns>The CUDA result</returns>
	static inline CUresult AllocateGPUMemory(MappedGPUData& retVal, const size_t dataSizeInBytes)
	{
		//Allocate memory in the GPU
		CUresult previousError = cuMemAlloc(&retVal.devicePointer, dataSizeInBytes);

		if(previousError == CUDA_SUCCESS)
		{
			//device pointer is filled by cuMemAlloc. Apply the last values
			retVal.bufferSize = dataSizeInBytes;
			retVal.isValid = true;
		}
		else
			retVal = MappedGPUData();

		return previousError;
	}

	///<summary>Allocates memory on the GPU for the CPU to use. Does not check if it has been allocated already</summary>
	///<param name="index">The index of the non-graphics resource to allocate</param>
	///<param name="dataSizeInBytes">The size in bytes of the data that will be allocated</param>
	///<returns>True if successful, false if not. If false, check previous error</returns>
	inline bool AllocateGPUMemory(const unsigned int index, const size_t dataSizeInBytes)
	{
		//Validate index and make sure it hasn't already been allocated
		if(index >= nonGraphicsResourceCount || cudaNonGraphicsResources[index].isValid)
			return false;

		//Allocate memory in the GPU
		previousError = cuMemAlloc(&cudaNonGraphicsResources[index].devicePointer, dataSizeInBytes);

		if(previousError == CUDA_SUCCESS)
		{
			//device pointer is filled by cuMemAlloc. Apply the last values
			cudaNonGraphicsResources[index].bufferSize = dataSizeInBytes;
			cudaNonGraphicsResources[index].isValid = true;
			return true;
		}
		else
		{
			//Just in case, reset everything to false
			cudaNonGraphicsResources[index].devicePointer = NULL;
			cudaNonGraphicsResources[index].bufferSize = 0;
			cudaNonGraphicsResources[index].isValid = false;
			return false;
		}
	}

	///<summary>Retrieves a non-graphics resource</summary>
	///<param name="index">The index to retrieve</param>
	///<returns>The resource requested, or an invalid structure if invalid index was provided</returns>
	inline const MappedGPUData GetNonGraphicsResource(const unsigned int index)
	{
		//Validate index
		if(index >= nonGraphicsResourceCount)
			return MappedGPUData();

		return cudaNonGraphicsResources[index];
	}

	inline CUdeviceptr GetNonGraphicsDevicePointer(const unsigned int index)
	{
		if(index >= nonGraphicsResourceCount)
			return NULL;

		return cudaNonGraphicsResources[index].devicePointer;
	}

	///<summary>Static helper method to copy host data to device</summary>
	///<param name="allocatedData">Data that has been previously allocated on the GPU</param>
	///<param name="srcData">The user data to copy</param>
	///<param name="dataSizeInBytes">Size of the data to copy</param>
	///<returns>Cuda error code or Invalid Handle if allocated data is not valid</returns>
	static inline CUresult CopyHostToDevice(MappedGPUData& allocatedData, const void* srcData)
	{
		if(allocatedData.isValid)
		{
			//Try the copy action
			CUresult previousError = cuMemcpyHtoDAsync_v2(allocatedData.devicePointer, 
				srcData, allocatedData.bufferSize, 0);
			return previousError;
		}
		else
			return CUDA_ERROR_INVALID_HANDLE;
	}

	///<summary>Copys data from the specifed Host data to the GPU device as specified by index. Uses
	///buffer size to determine how many bytes to copy over</summary>
	///<param name="index">The index of the device to copy the data to</param>
	///<param name="data">The data to be copied over</param>
	///<returns>True if successful, false if not. If false, check previous error</returns>
	inline bool CopyHostToDevice(const unsigned int index, const void* srcData)
	{
		//Validate index
		if(index >= nonGraphicsResourceCount)
			return false;

		if(cudaNonGraphicsResources[index].isValid)
		{
			//Try the copy action
			//previousError = cuMemcpyHtoD_v2(cudaNonGraphicsResources[index].devicePointer, srcData, dataSizeInBytes);
			previousError = cuMemcpyHtoDAsync_v2(cudaNonGraphicsResources[index].devicePointer, 
				srcData, cudaNonGraphicsResources[index].bufferSize, 0);
			return previousError == CUDA_SUCCESS;
		}
		else
			return false;
	}

	///<summary>Copys data from the specifed Host data to the GPU device as specified by index, but
	///allows specifying how much data to copy over</summary>
	///<param name="index">The index of the device to copy the data to</param>
	///<param name="data">The data to be copied over</param>
	///<param name="dataSizeInBytes">The size of the data in bytes to be copied over</param>
	///<returns>True if successful, false if not. If false, check previous error</returns>
	inline bool CopyHostToDevice(const unsigned int index, const void* srcData, const size_t dataSizeInBytes)
	{
		//Validate index
		if(index >= nonGraphicsResourceCount)
			return false;

		if(cudaNonGraphicsResources[index].isValid)
		{
			//Try the copy action
			//previousError = cuMemcpyHtoD_v2(cudaNonGraphicsResources[index].devicePointer, srcData, dataSizeInBytes);
			previousError = cuMemcpyHtoDAsync_v2(cudaNonGraphicsResources[index].devicePointer, srcData, dataSizeInBytes, 0);
			return previousError == CUDA_SUCCESS;
		}
		else
			return false;
	}

	///<summary>Copies the data from the specified device to the specified host location</summary>
	///<param name="index">The index of the device to copy the data from</param>
	///<param name="data">The location for the data to be copied to</summary>
	///<param name="dataSizeInBytes>The size of the data in bytes to be copied over</summary>
	///<returns>True if successful, false if not. If false, check previous error</returns>
	inline bool CopyDeviceToHost(const unsigned int index, void* destData, const size_t dataSizeInBytes)
	{
		//Validate index
		if(index >= nonGraphicsResourceCount)
			return false;

		if(cudaNonGraphicsResources[index].isValid)
		{
			//Perform the copy
			previousError = cuMemcpyDtoH_v2(destData, cudaNonGraphicsResources[index].devicePointer, dataSizeInBytes);
			return previousError == CUDA_SUCCESS;
		}
		else return false;
	}

	///<summary>Convenience function to perform allocation and copy in one method</summary>
	///<param name="index">The index of the device pointer to allocate and copy data to</param>
	///<param name="allocationSize">The amount of data in bytes to allocate on the GPU</param>
	///<param name="srcData">The source of the data to be copied over</param>
	///<param name="dataSize">Size of the data to be copied over, in bytes</param>
	///<returns>True if successful, false if not. If false, check previous error. This method can potentially hide
	///allocation errors however.<returns>
	inline bool AllocateAndCopyToDevice(const unsigned int index, const size_t allocationSize,
								const void* srcData, const size_t dataSize)
	{
		//Try to allocate the memory
		if(AllocateGPUMemory(index, allocationSize))
			//Try to copy the data over
			return CopyHostToDevice(index, srcData, dataSize);
		else
			return false;
	}

	///<summary>Static helper method to free allocated Device memory</summary>
	///<param name="allocatedData">The previously allocated data</param>
	///<returns>Cuda result, or Cuda_Invalid_Handle if allocated Data was invalid</returns>
	static inline CUresult FreeGPUMemory(MappedGPUData& allocatedData)
	{
		//If we are capable of freeing the pointer
		if(allocatedData.isValid)
		{
			CUresult prevError = cuMemFree(allocatedData.devicePointer) ;
			
			if(prevError == CUDA_SUCCESS)
				allocatedData = MappedGPUData();//reset values
			
			return prevError;
		}

		return CUDA_ERROR_INVALID_HANDLE;
	}

	///<summary>Frees the allocated memory on the GPU as represented by the passed index</summary>
	///<param name="index">The index of the non-graphics resource to allocate</param>
	///<returns>True if successful, false if not. If false, check previous error</returns>
	inline bool FreeGPUMemory(const unsigned int index)
	{
		//Validate index
		if(index >= nonGraphicsResourceCount)
			return false;

		//If we are capable of freeing the pointer
		if(cudaNonGraphicsResources[index].isValid)
		{
			//Attempt to free
			previousError = cuMemFree(cudaNonGraphicsResources[index].devicePointer);

			if(previousError == CUDA_SUCCESS)
			{
				//Does cuMemFree set device pointer to null?
				cudaNonGraphicsResources[index].devicePointer = NULL;
				cudaNonGraphicsResources[index].bufferSize = 0;
				cudaNonGraphicsResources[index].isValid = false;

				return true;
			}
		}

		return false;
	}

	///<summary>Unsafe Free convenience method. Unsafe because it does not indicate which resource failed to be freed</summary>
	///<returns>An integer representing how many resources were freed. Does not indicate which one failed if
	///it doesn't match with the number of Non-Graphics resources<returns>
	inline unsigned int FreeAllGPUMemory()
	{
		unsigned int amountFreed = 0;

		for (unsigned int i = 0; i < nonGraphicsResourceCount; i++)
		{
			//Attempt to free
			if(FreeGPUMemory(i))
				++amountFreed;
		}
	
		//Return results
		return amountFreed;
	}

#pragma endregion

};

