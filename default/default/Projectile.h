#pragma once
#include "entity.h"

class Projectile :
	public Entity
{
	glm::vec3 originalVelocity;

public:
	glm::vec3 target_pos;
	Projectile(glm::vec3 t_p, glm::vec3 p, glm::vec3 s, glm::vec3 a, glm::vec3 o_v, glm::vec3 colour = { -1.f,-1.f,-1.f });
	bool Projectile::inTargetRange(glm::vec3 vel);
};

