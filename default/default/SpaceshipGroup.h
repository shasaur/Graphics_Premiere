#pragma once
#include "entitygroup.h"
#include "scene.h"
#include "Projectile.h"

class SpaceshipGroup : public EntityGroup
{
	// Update logic
	void FireWeapons();
	void init();

public:
	// Constructors
	SpaceshipGroup(Entity e);
	SpaceshipGroup(glm::vec3 p, glm::vec3 s, glm::vec3 a, std::map<GLuint, MatInfo> matMap,
		std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<GLuint> texture_ids, std::vector<glm::vec2> texture_coords);

	// Logistics
	void Update();

	// Weapon logic
	std::vector<Weapon> weapons;
	Entity* weapon_target;
	Scene* spawn_scene; // where projectiles will be added to
	virtual void AddWeaponSystem(std::vector<Weapon> rates, Entity* g, Scene* s);
};

