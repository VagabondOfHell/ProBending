#pragma once
#include <vector>

namespace AttackEnum
{
#pragma region Earth Attacks
	enum EarthAttacks
	{
		STOMP_BLAST,
		EARTH_ATTACK_COUNT
	};

	static std::string EarthAttackToString(EarthAttacks attack)
	{
		switch (attack)
		{
		case AttackEnum::STOMP_BLAST:
			return "Stomp Blast";
			break;
		default:
			return "";
			break;
		}
	}

	enum StompBlastProgress
	{
		LEG_UP,
		LEG_DOWN,
		STOMP_BLAST_PROGRESS_COUNT
	};

#pragma endregion

#pragma region Fire Attacks

	enum FireAttacks
	{
		FIRE_JAB,
		FIRE_ATTACK_COUNT
	};

#pragma endregion

#pragma region WaterAttacks

	enum WaterAttacks
	{
		WATER_ATTACK_COUNT
	};

#pragma endregion

};

class GestureChain;

class GestureObserver
{
public:
	virtual void GestureCompleted(const GestureChain& gestureCompleted) = 0;
	virtual void GestureReset(const GestureChain& gestureReset) = 0;
};

class GestureChain
{
private:
	std::string name;

	float timePassed;
	std::vector<bool> gestureStatus;
	float timeToComplete;
	unsigned int currentIndex;
	
public:
	GestureObserver* observer;

	GestureChain(const std::string& _name, float _timeToComplete, unsigned int numGestureStatus, GestureObserver* _observer = NULL)
		:timePassed(0.0f), timeToComplete(_timeToComplete), currentIndex(0), name(_name), observer(_observer)
	{
		for (unsigned int i = 0; i < numGestureStatus; i++)
		{
			gestureStatus.push_back(false);
		}
	}

	inline const std::string& GetName()const{return name;}

	inline bool ChainCompleted()const
	{
		return currentIndex >= gestureStatus.size() - 1;
	}

	inline void SetNextGestureStatus()
	{
		gestureStatus[currentIndex] = true;

		if(currentIndex < gestureStatus.size() - 1)
			++currentIndex;
		else
			if(observer)
				observer->GestureCompleted(*this);
	}

	inline void ResetChain()
	{
		timePassed = 0.0f;
		currentIndex = 0;

		for (unsigned int i = 0; i < gestureStatus.size(); i++)
		{
			gestureStatus[i] = false;
		}

		if(observer)
			observer->GestureReset(*this);
	}

	inline void Update(float gameTime)
	{
		if(currentIndex > 0)
		{
			timePassed += gameTime;

			if(timePassed >= timeToComplete)
				ResetChain();
		}
		
	}
};