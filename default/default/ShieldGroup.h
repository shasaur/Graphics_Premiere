#pragma once
#include "entitygroup.h"
class ShieldGroup : public EntityGroup
{
	int ticksLeft;
	int maxTicks;

public:
	ShieldGroup(Type t, glm::vec3 p, glm::vec3 s, glm::vec3 a);
	virtual std::vector<Entity*> Render();
};

