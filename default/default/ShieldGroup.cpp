#include "ShieldGroup.h"



ShieldGroup::ShieldGroup(Type t, glm::vec3 p, glm::vec3 s, glm::vec3 a)
	: EntityGroup(t, p, s, a)
{
	maxTicks = 100;
	ticksLeft = maxTicks;
}

std::vector<Entity*> ShieldGroup::Render() {
	std::vector<Entity*> entitiesToDraw = std::vector<Entity*>();

	ticksLeft--;
	int i = (en.size()-1)-(int)(floor(((float)ticksLeft/(float)maxTicks) * (float)SHIELD_RIPPLE_DETAIL));
	if (i < en.size() && i >= 0) {
		entitiesToDraw.push_back(&en.at(i));

	}
	else {
		entitiesToDraw.push_back(nullptr);
	}

	return entitiesToDraw;
}