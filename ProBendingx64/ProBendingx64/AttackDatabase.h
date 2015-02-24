#pragma once
#include <vector>

class Attack;

class AttackDatabase
{
private:
	
public:
	static void GetEarthAttacks(std::vector<Attack>& outVal);
	static void GetFireAttacks(std::vector<Attack>& outVal);
	static void GetAirAttacks(std::vector<Attack>& outVal);
	static void GetWaterAttacks(std::vector<Attack>& outVal);
};

