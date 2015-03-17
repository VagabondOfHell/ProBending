#include "CudaModuleHelper.h"
#include "cuda.h"

CUresult CudaModuleHelper::previousCudaError = CUDA_SUCCESS;

CudaModuleHelper::CudaModuleHelper(void)
{
}


CudaModuleHelper::~CudaModuleHelper(void)
{
}

bool CudaModuleHelper::LoadCUDAModule(CUmodule* module, std::string fileName)
{
	//open the file
	FILE *fp;
	errno_t error = fopen_s(&fp, "KernelTest.ptx", "rb");

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

		//store the previous cuda error so that if this method returns false,
		//there can be some indication as to why
		previousCudaError = cuModuleLoadData(module, buf );

		//Now we are done with the character array representing the kernel, so delete it
		delete[] buf;
		
		//Return success as a boolean
		return previousCudaError == CUDA_SUCCESS;
	}

	//if we reach here we couldn't open the file, so we will throw an INVALID IMAGE error
	previousCudaError = CUDA_ERROR_INVALID_IMAGE;

	//Indicate failure
	return false;
}
