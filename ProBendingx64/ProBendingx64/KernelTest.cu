#pragma once
#ifndef _DEBUG
#define _DEBUG
#endif

#include <stdio.h>
#include "cuda_runtime.h"
#include "curand_kernel.h"
#include "AffectorParameters.h"

#include "device_launch_parameters.h"

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

// copies positions and alpha to the destination vertex buffer based on 
// validity bitmap and particle life times
extern "C" __global__ void UpdateParticleSystem(
	GPUResourcePointers graphicsResources,
	PxVec4* srcPositions, 
	PxU32* validParticleBitmap,
	float* srcLifetimes,
	float initialLifetime,
	GPUParamsCollection* affectorParams,
	PxU32 validParticleRange)
{
	if (!threadIdx.x)
		gOffset = 0;

	__syncthreads();

	if (validParticleRange)
	{
		for (PxU32 w=threadIdx.x; w <= (validParticleRange - 1) >> 5; w+=blockDim.x)
		{
			const PxU32 srcBaseIndex = w << 5;

			// reserve space in the output vertex buffer based on
			// population count of validity bitmap (avoids excess atomic ops)
			PxU32 destIndex = atomicAdd(&gOffset, __popc(validParticleBitmap[w]));

			for (PxU32 b=validParticleBitmap[w]; b; b &= b-1) 
			{
				PxU32 index = srcBaseIndex | __ffs(b)-1;

				const PxU32 offset = destIndex*sizeof(PxVec4);

				// copy position, save the w
				PxVec4* p = ptrOffset(graphicsResources.positions, offset);
				p->x = srcPositions[index].x;
				p->y = srcPositions[index].y;
				p->z = srcPositions[index].z;

				//If there are any affectors to apply
				if(affectorParams)
				{
					const PxU32 lifeOffset = index * sizeof(float);
					float* l = ptrOffset(srcLifetimes, lifeOffset);
					float percent = (*l / initialLifetime);

					if(affectorParams->colourFadeParams)
					{
						//Need to use colours
						PxVec4* c = ptrOffset(graphicsResources.primaryColour, offset);

						c->x = affectorParams->colourFadeParams->endColour.x - (affectorParams->colourFadeParams->colourDifference.x * percent);
						c->y = affectorParams->colourFadeParams->endColour.y - (affectorParams->colourFadeParams->colourDifference.y * percent);
						c->z = affectorParams->colourFadeParams->endColour.z - (affectorParams->colourFadeParams->colourDifference.z * percent);
						c->w = affectorParams->colourFadeParams->endColour.w - (affectorParams->colourFadeParams->colourDifference.w * percent);
					}

					if(affectorParams->scaleParameters)
					{
						if(affectorParams->scaleParameters->enlarge)
							p->w = affectorParams->scaleParameters->maxScale - (affectorParams->scaleParameters->scaleDiff * percent);
						else
							p->w = affectorParams->scaleParameters->minScale + (affectorParams->scaleParameters->scaleDiff * percent);
					}
				}
				
				++destIndex;
			}
		}
	}
}