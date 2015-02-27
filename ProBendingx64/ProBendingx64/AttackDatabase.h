#pragma once
#include <vector>

class ProjectileManager;
class Attack;

class AttackDatabase
{
private:
	
public:
	static void GetEarthAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal);
	static void GetFireAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal);
	static void GetAirAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal);
	static void GetWaterAttacks(ProjectileManager* projManager, std::vector<Attack>& outVal);
};

