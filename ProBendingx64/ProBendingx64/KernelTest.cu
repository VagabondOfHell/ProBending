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
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	
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

// copies positions and alpha to the destination vertex buffer based on 
// validity bitmap and particle life times
extern "C" __global__ void updateBillboardVB(
	PxVec3* destPositions,
	const PxVec4* srcPositions, 
	float* lifetimes,
	const PxU32* validParticleBitmap,
	PxU32 validParticleRange)
{
	for (int i = threadIdx.x; i < validParticleRange; i+=blockDim.x)
	{
		destPositions[i].x = srcPositions[i].x;
		destPositions[i].y = srcPositions[i].y;
		destPositions[i].z = srcPositions[i].z;
	}

	for (int i = threadIdx.x; i < 100000; i+=blockDim.x)
	{
		if(lifetimes[i] <= 0)
		{
			lifetimes[i] = 0.0f;
			destPositions[i].x = 0.0f;
			destPositions[i].y = 0.0f;
			destPositions[i].z = 0.0f;
		}
	}
	//if (!threadIdx.x)
	//	gOffset = 0;

	//__syncthreads();

	//if (validParticleRange)
	//{
	//	for (PxU32 w=threadIdx.x; w <= (validParticleRange) >> 5; w+=blockDim.x)
	//	{
	//		const PxU32 srcBaseIndex = w << 5;

	//		// reserve space in the output vertex buffer based on
	//		// population count of validity bitmap (avoids excess atomic ops)
	//		PxU32 destIndex = atomicAdd(&gOffset, __popc(validParticleBitmap[w]));

	//		for (PxU32 b=validParticleBitmap[w]; b; b &= b-1) 
	//		{
	//			PxU32 index = srcBaseIndex | __ffs(b)-1;

	//			const PxU32 offset = destIndex*12;

	//			// copy position
	//			PxVec3* p = ptrOffset(destPositions, offset);
	//			p->x = srcPositions[index].x;
	//			p->y = srcPositions[index].y;
	//			p->z = srcPositions[index].z;

	//			/*if(srcPositions[index].x >= 12)
	//				srcLifetimes[index] = 0.0f;*/

	//			++destIndex;
	//		}
	//	}
	//}
}

