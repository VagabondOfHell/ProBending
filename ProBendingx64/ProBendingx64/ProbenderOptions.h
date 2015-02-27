#pragma once

struct ProbenderOptions
{
	float LeanThreshold; //Amount a lean must surpass to be registered as true
	float JumpThreshold; //Amount a jump must surpass to be registered as true

	float d;
	ProbenderOptions(float leanThreshold = 0.1f, float jumpThreshold = 0.10f)
		:LeanThreshold(leanThreshold), JumpThreshold(jumpThreshold)
	{

	}

	~ProbenderOptions()
	{

	}
};