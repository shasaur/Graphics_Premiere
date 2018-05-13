#include "SpaceshipGroup.h"



SpaceshipGroup::SpaceshipGroup(Entity e) : EntityGroup(e) {
	weapon_target = nullptr;
	weapons = std::vector<Weapon>();
	spawn_scene = nullptr;
}

void SpaceshipGroup::AddWeaponSystem(std::vector<Weapon> w, Entity* t, Scene* s) {
	for (int i = 0; i < w.size(); i++) {
		w.at(i).position += centre;
		weapons.push_back(w.at(i));
		printf("\ncentre (%f,%f,%f)\n", centre.x, centre.y, centre.z);
		printf("weapon pos (%f,%f,%f)\n", w.at(i).position.x, w.at(i).position.y, w.at(i).position.z);
	}
	weapon_target = t;
	spawn_scene = s;
}



void SpaceshipGroup::FireWeapons() {
	for (int w = 0; w < weapons.size(); w++) {
		float speed = 0.5f;
		float fire_threshold = (1.f / 60.f) * weapons.at(w).rate;
		float fire_probability = (float)(rand() % 100) / 100.f;

		if (fire_probability <= fire_threshold) {
			Entity projectile(Entity::Sphere, weapons.at(w).position, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), 20, false, { 1.f, 0.5f, 1.f });
			
			double offset_amplifier = 2.0;
			glm::dvec3 randomOffset((((rand() % 100) / 100.f)*weapon_target->size.x*offset_amplifier) - (weapon_target->size.x / 2.0),
									(((rand() % 100) / 100.f)*weapon_target->size.y*offset_amplifier) - (weapon_target->size.y / 2.0),
									(((rand() % 100) / 100.f)*weapon_target->size.z*offset_amplifier) - (weapon_target->size.z / 2.0));
			projectile.SetVelocity((weapon_target->position+randomOffset) - glm::dvec3(weapons.at(w).position));
			spawn_scene->AddProjectile(projectile);
			printf("fire\n");
		}
	}
}

void SpaceshipGroup::Update() {
	FireWeapons();
}