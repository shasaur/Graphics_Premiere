#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Weapon
{
public:
	enum Type {Cannon, Laser, Minicannon};
	
	Weapon(glm::vec3 p, Type t, float r);
	~Weapon();

	glm::vec3 position;
	float rate;
	Type type;
};

