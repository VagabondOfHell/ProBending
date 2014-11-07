#pragma once
#ifndef _DEBUG
#define _DEBUG
#endif

#include <stdio.h>
#include "cuda_runtime.h"
#include "curand_kernel.h"
#include "foundation\PxVec2.h"
#include "foundation\PxVec3.h"
#include "foundation\PxVec4.h"
#include "device_launch_parameters.h"
//
using namespace physx;

template <typename T>
__device__ T* ptrOffset(T* p, PxU32 byteOffset)
{
	return (T*)((unsigned char*)(p) + byteOffset);
}

#if __CUDA_ARCH__ < 200
__device__ PxU32 gOffset;
#else
__device__ __shared__ PxU32 gOffset;
#endif


__global__ void addKernel(int *c, const int *a, const int *b)
{
    int i = threadIdx.x;
    c[i] = a[i] + b[i];
}
//TODO: Take in bitmap and check validity of each particle
//TODO: Pass in the GL Buffer for colour and Emiiter position and modify colour based on distance
extern "C"__global__ void UpdateParticlesKernel(PxVec3* destPositions, PxVec4* srcPositions)
{
	int i =  blockIdx.x * blockDim.x + threadIdx.x;
	
	if(srcPositions[i].magnitude() > 25.0f)
	{
		srcPositions[i].x = 0;		
		srcPositions[i].y = 0;	
		srcPositions[i].z = 0;		
	}
	
	destPositions[i].x = srcPositions[i].x;		
	destPositions[i].y = srcPositions[i].y;	
	destPositions[i].z = srcPositions[i].z;	
}

