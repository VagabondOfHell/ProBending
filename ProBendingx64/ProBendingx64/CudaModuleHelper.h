#pragma once
#include <string>
#include "cuda.h"

class CudaModuleHelper
{
private:
	static CUresult previousCudaError;

public:
	CudaModuleHelper(void);
	~CudaModuleHelper(void);

	///<summary>Loads a CUDA Module</summary>
	///<param name = "mod">The OUT value of the CUDA module pointer</param>
	///<param name = "filename">The name of the file WITH EXTENSION to load. .PTX, .FATBIN or .CUBIN</param>
	///<returns>True if successful, false if not. If false, call GetLastCudaError to see why</returns>
	static bool LoadCUDAModule(CUmodule* mod, std::string fileName);

	///<summary> Loads a CUDA function from the specified module </summary>
	///<param name = "function"> The OUT value of the CUDA function pointer </param>
	///<param name = "module"> The module to search for the function in (Must be previously loaded with a call to LoadCUDAModule) </param>
	///<param name = "kernelFunctionName"> The name of the function to find</param>
	///<returns> True if successful, false if not. If false, call GetLastCudaError to see why </returns>
	static inline bool LoadCUDAFunction(CUfunction* function, CUmodule module, std::string kernelFunctionName)
	{
		//Attempt to get the function from the module
		previousCudaError = cuModuleGetFunction(function, module, kernelFunctionName.c_str());

		return previousCudaError == CUDA_SUCCESS;
	}

	///<summary> Allocates memory in the Device (GPU) for usage and copys the data from Host to the Device </summary>
	///<param name = "deviceAddress"> The OUT value of the device pointer to fill </param>
	///<param name = "allocSizeInBytes"> The size in bytes to allocate on the Device </param>
	///<param name = "data"> The data to fill the Device Pointer with </param>
	///<param name = "dataSizeInBytes"> The size in bytes to copy to the Device from the Host </param>
	///<returns> True if successful, false if not. If false, call GetLastCudaError to see why </returns>
	static inline bool AllocateAndCopyHostToDevice(CUdeviceptr* deviceAddress, size_t allocSizeInBytes, 
		const void* data, size_t dataSizeInBytes)
	{
		//Try to allocate the memory on the device and if successful, copy the desired data to the device.
		//If unsuccessful, return false as the pointer hasn't been allocated and therefore allows user to retrieve the error
		if(AllocateDeviceMemory(deviceAddress, allocSizeInBytes))
			return CopyMemoryHostToDevice(*deviceAddress, data, dataSizeInBytes);
		else
			return false;
	}

	///<summary> Allocates memory in the Device (GPU) for usage </summary>
	///<param name = "deviceAddress"> The OUT value of the device pointer to fill </param>
	///<param name = "sizeInBytes"> The size in bytes to allocate on the Device </param>
	///<returns> True if successful, false if not. If false, call GetLastCudaError to see why </returns>
	static inline bool AllocateDeviceMemory(CUdeviceptr* deviceAddress, size_t sizeInBytes)
	{
		//Allocate memory in the GPU
		previousCudaError = cuMemAlloc(deviceAddress, sizeInBytes);

		//Check error status
		return previousCudaError == CUDA_SUCCESS;
	}

	///<summary> Copies data from the Host device (CPU) to the Destination device (GPU) </summary>
	///<param name = "destinationDevice"> The allocated memory address on the GPU to fill </param>
	///<param name = "data"> The pointer to the data to be copied over </param>
	///<param name = "sizeInBytes"> The size of the data to copy over in bytes </param>
	///<returns> True if successful, false if not. If false, call GetLastCudaError to see why </returns>
	static inline bool CopyMemoryHostToDevice(CUdeviceptr destinationDevice, const void* data, size_t sizeInBytes)
	{
		//Try the copy action
		previousCudaError = cuMemcpyHtoD_v2(destinationDevice, data, sizeInBytes);

		//Indicate success or failure
		return previousCudaError == CUDA_SUCCESS;
	}

	///<summary>Gets the last reported Cuda error. Best called when a method returns false </summary>
	///<returns>The CUDA enum Error Result</returns>
	static inline CUresult GetLastCudaError()
	{
		return previousCudaError;
	}

};

