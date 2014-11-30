#pragma once
#include <memory>

class AbilityDescriptor;

typedef std::shared_ptr<AbilityDescriptor> SharedAbilityDescriptor;

class Projectile
{
private:
	SharedAbilityDescriptor attachedAbility;

public:
	
	Projectile(SharedAbilityDescriptor _attachedAbility);
	virtual ~Projectile(void);
};

