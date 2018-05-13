#pragma once
#include "entitygroup.h"
#include "scene.h"

class SpaceshipGroup : public EntityGroup
{
	// Update logic
	void FireWeapons();

public:
	// Constructors
	SpaceshipGroup(Entity e);

	// Logistics
	void Update();

	// Weapon logic
	std::vector<Weapon> weapons;
	Entity* weapon_target;
	Scene* spawn_scene; // where projectiles will be added to
	virtual void AddWeaponSystem(std::vector<Weapon> rates, Entity* g, Scene* s);
};

