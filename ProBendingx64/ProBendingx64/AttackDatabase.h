#pragma once
#include <vector>

class ProjectileManager;
class Attack;

class AttackDatabase
{
private:
	
public:
	static void GetEarthAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal);
	static void GetFireAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal);
	static void GetAirAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal);
	static void GetWaterAttacks(ProjectileManager* projManager, unsigned short contestantID, std::vector<Attack>& outVal);
};

