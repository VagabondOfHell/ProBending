#pragma once
#include <map>
#include <string>
#include <cuda.h>

struct CudaModule
{
	friend class CudaModuleManager;

private:
	typedef std::pair<std::string, CUfunction> functionMapPair;
	typedef std::map<std::string, CUfunction>::iterator functionMapIter;
	
	std::map<std::string, CUfunction> functionMap;

	CUmodule cudaModule;

public:

	CudaModule(){}
	~CudaModule(){}

	CUmodule GetCudaModule(){return cudaModule;}

	///<summary>Loads a Cuda Function using the Module assigned to this struct, or returns the
	///function if it has already been loaded</summary>
	///<param name="functionName">The name of the function to be loaded</param>
	///<returns>The function loaded, the function that already existed, or NULL if loading failed</returns>
	CUfunction LoadFunction(std::string functionName)
	{
		CUfunction function;

		//If the cuda module exists
		if(cudaModule)
		{
			//check find results
			functionMapIter result = functionMap.find(functionName); 

			//if find was not successful
			if(result == functionMap.end())
			{
				//Attempt to get the function from the module
				CUresult previousCudaError = cuModuleGetFunction(&function, cudaModule, functionName.c_str());

				//If cuda was successful
				if(previousCudaError == CUDA_SUCCESS)
				{
					//insert the results
					functionMap.insert(functionMapPair(functionName, function));
					return function;
				}
			}
			else
				return result->second;
		}
		
		//If we reach here, return null
		return NULL;
	}

	///<summary>Gets the function of the specified name</summary>
	///<param name="functionName">The name of the function to retrieve</param>
	///<returns>The cuda function, or NULL if it doesn't exist</returns>
	CUfunction GetFunction(std::string functionName)
	{
		functionMapIter searchResults = functionMap.find(functionName);

		if(searchResults != functionMap.end())
			return searchResults->second;

		return NULL;
	}
};