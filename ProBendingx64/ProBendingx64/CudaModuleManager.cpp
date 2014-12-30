#include "CudaModuleManager.h"

CudaModuleManager* CudaModuleManager::instance;

CudaModuleManager::CudaModuleManager(void)
{

}

CudaModuleManager::~CudaModuleManager(void)
{
	UnloadAllModules();
}

CudaModule* CudaModuleManager::LoadModule(std::string moduleFilepath)
{
	ModuleMapIter searchResult = moduleMap.find(moduleFilepath);

	//if found, return it
	if(searchResult != moduleMap.end())
		return &searchResult->second;

	//open the file
	FILE *fp;
	errno_t error = fopen_s(&fp, moduleFilepath.c_str(), "rb");

	if(!error)
	{
		//Seek the end of the file
		fseek(fp, 0, SEEK_END);
		//Get the size of the file (we are now at the end, so we have access to the information)
		int file_size = ftell(fp);
		//Create a char* array of the file size + the null-terminating string
		char *buf = new char[file_size+1];
		//Seek the beginning
		fseek(fp, 0, SEEK_SET);
		//Read all the data into the buffer
		fread(buf, sizeof(char), file_size, fp);
		//Close the file
		fclose(fp);
		//Append a NULL-terminator
		buf[file_size] = '\0';

		CUmodule module;

		//store the previous cuda error so that if this method returns false,
		//there can be some indication as to why
		previousCudaError = cuModuleLoadData(&module, buf );

		//Now we are done with the character array representing the kernel, so delete it
		delete buf;

		//if successful
		if(previousCudaError == CUDA_SUCCESS)
		{
			//Insert the module into the map
			CudaModule cudaModule = CudaModule();
			cudaModule.cudaModule = module;
			std::pair<ModuleMapIter,bool> insertResult =
				moduleMap.insert(ModuleMapPair(moduleFilepath, cudaModule));

			if(insertResult.second)
				//return the insertion results module if insertion was successful
				return &insertResult.first->second;
		}
	}

	//if we reach here we couldn't open the file, so we will throw an INVALID FILE error
	previousCudaError = CUDA_ERROR_FILE_NOT_FOUND;

	//Indicate failure
	return NULL;
}

bool CudaModuleManager::UnloadModule(std::string moduleFilepath)
{
	//Search for the specified module
	ModuleMapIter searchResult = moduleMap.find(moduleFilepath);

	//if found
	if(searchResult != moduleMap.end())
	{
		//Try unload
		previousCudaError = cuModuleUnload(searchResult->second.cudaModule);

		if(previousCudaError == CUDA_SUCCESS)
		{
			//If successful, remove from map
			moduleMap.erase(searchResult);
			return true;
		}
	}

	return false;
}

void CudaModuleManager::UnloadAllModules()
{
	//Loop through and unload all modules
	for (ModuleMapIter start = moduleMap.begin(); start != moduleMap.end(); ++start)
	{
		cuModuleUnload(start->second.cudaModule);
	}

	//Clear the map
	moduleMap.clear();
}

CudaModule* CudaModuleManager::GetModule(std::string moduleFilepath)
{
	ModuleMapIter searchResults = moduleMap.find(moduleFilepath);

//if found, return it
	if(searchResults != moduleMap.end())
		return &searchResults->second;

	return NULL;
}

