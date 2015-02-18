#pragma once

struct ProbenderOptions
{
	float LeanThreshold; //Amount a lean must surpass to be registered as true
	float JumpThreshold; //Amount a jump must surpass to be registered as true

	ProbenderOptions(float leanThreshold = 0.1f, float jumpThreshold = 1.0f)
		:LeanThreshold(leanThreshold), JumpThreshold(jumpThreshold)
	{

	}

	~ProbenderOptions()
	{

	}
};