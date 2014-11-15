#include "cuda_runtime.h"
#include "PxPhysics.h"
#include "foundation\PxVec2.h"
#include "foundation\PxVec3.h"
#include "foundation\PxVec4.h"

using namespace physx;
//
//cudaError_t addWithCuda(int *c, const int *a, const int *b, unsigned int size);
//
//extern "C" __global__ void UpdateParticlesKernel(PxVec3* destPositions, PxU32 destStride,	const PxVec4* srcPositions,
//	const PxU32* validParticleBitmap, PxU32 validParticleRange);

cudaError_t LaunchUpdateParticlesKernel(PxVec3* destPositions, PxU32 destStride, const PxVec4* srcPositions,
	const PxU32* validParticleBitmap, PxU32 validParticleRange);