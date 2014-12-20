#pragma once
#include <map>
#include "CudaModule.h"

class CudaModuleManager
{
private:
	static CudaModuleManager* instance;

	typedef std::pair<std::string, CudaModule> ModuleMapPair;
	typedef std::map<std::string, CudaModule>::iterator ModuleMapIter;

	std::map<std::string, CudaModule> moduleMap;

	CudaModuleManager(void);

	CUresult previousCudaError;

public:
	
	~CudaModuleManager(void);

	///<summary>Gets the singleton instance of the manager</summary>
	///<returns>Pointer to this singleton instance</returns>
	static CudaModuleManager* const GetSingleton()
	{
		if(!instance)
			instance = new CudaModuleManager();

		return instance;
	}

	///<summary>Destroys the singleton instance</summary>
	void DestroySingleton()
	{
		if(instance)
		{
			delete instance;
			instance = NULL;
		}
	}

	///<summary>Gets the last cuda error that was recorded</summary>
	///<returns>Last Cuda Error</returns>
	CUresult GetLastCudaError()const{return previousCudaError;}

	///<summary>Gets the module of the specified name</summary>
	///<param name="moduleFilepath">The filepath of the module, which is used as the name</param>
	///<returns>The module, or NULL if not found</returns>
	CudaModule* GetModule(std::string moduleFilepath);

	///<summary>Loads a Cuda Module, or returns the existing one</summary>
	///<param name="moduleFilepath">The filepath of the module, also used as the name</param>
	///<returns>Cuda Module structure, or NULL if failure</returns>
	CudaModule* LoadModule(std::string moduleFilepath);

	///<summary>Unloads a module</summary>
	///<param name="moduleFilepath">The name of the module</param>
	///<returns>True if successful, false if not</returns>
	bool UnloadModule(std::string moduleFilepath);

	///<summary>Unloads all modules stored in the manager</summary>
	void UnloadAllModules();
};

