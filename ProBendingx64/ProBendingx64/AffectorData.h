#pragma once
#include "ParticleAffectorEnum.h"
#include "ParticleAffectors.h"

#include <map>
#include <memory>

typedef std::map<ParticleAffectorType::ParticleAffectorType, std::shared_ptr<ParticleAffector>> AffectorMap;
typedef std::pair<AffectorMap::iterator, bool> AffectorMapInsertResult;

struct AffectorData
{
	ParticleAffectorType::ParticleAffectorFlag gpuTypeCombination; //Flag of affectors on GPU
	ParticleAffectorType::ParticleAffectorFlag allTypesCombination; //Flag of all affectors

	AffectorMap affectorMap;//list of affectors organized by type

	bool hostAffector; //True if there is an affector on the CPU

	AffectorData()
		: gpuTypeCombination(0), allTypesCombination(0), hostAffector(false)
	{
	}

	inline AffectorMap::iterator GetMapBegin(){return affectorMap.begin();}

	inline AffectorMap::iterator GetMapEnd(){return affectorMap.end();}
};