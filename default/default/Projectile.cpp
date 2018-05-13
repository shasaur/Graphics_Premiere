#include "Projectile.h"



Projectile::Projectile(glm::vec3 t_p, glm::vec3 p, glm::vec3 s, glm::vec3 a, glm::vec3 o_v, glm::vec3 colour)
	: Entity(Entity::Sphere, p, s, a, 20, colour)
{
	target_pos = glm::vec3(t_p);
	originalVelocity = glm::vec3(o_v);
}

bool Projectile::inTargetRange(glm::vec3 vel) {
	if (originalVelocity.x != vel.x)
		return true;

	return false;
}