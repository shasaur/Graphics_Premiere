#include "Weapon.h"



Weapon::Weapon(glm::vec3 p, Type t, float r)
{
	position = p;
	type = t;
	rate = r;
}


Weapon::~Weapon()
{
}
