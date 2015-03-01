#include "RandomNumberGenerator.h"

RandomNumberGenerator* RandomNumberGenerator::instance = NULL;

RandomNumberGenerator::RandomNumberGenerator(void)
{

}

RandomNumberGenerator::~RandomNumberGenerator(void)
{
}

float RandomNumberGenerator::GenerateRandom(float min, float max)
{
	std::uniform_real_distribution<float> val(min, max);

	return val(eng);
}

