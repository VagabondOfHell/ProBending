#pragma once
#include <random>

class RandomNumberGenerator
{
private:
	static RandomNumberGenerator* instance;

	std::random_device rd;
	std::mt19937_64 eng;

	RandomNumberGenerator(void);

public:
	~RandomNumberGenerator(void);

	static inline RandomNumberGenerator* GetInstance()
	{
		if(!instance)
			instance = new RandomNumberGenerator();

		return instance;
	}

	static void DestroyInstance()
	{
		if(instance)
		{
			delete instance;
			instance = nullptr;
		}
	}

	float GenerateRandom(float min, float max);

};

