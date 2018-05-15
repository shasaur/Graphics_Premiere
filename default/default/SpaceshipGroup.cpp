#include "SpaceshipGroup.h"

void SpaceshipGroup::init() {
	weapon_target = nullptr;
	weapons = std::vector<Weapon>();
	spawn_scene = nullptr;
}

SpaceshipGroup::SpaceshipGroup(Entity e) : EntityGroup(e) {
	init();
}

SpaceshipGroup::SpaceshipGroup(glm::vec3 p, glm::vec3 s, glm::vec3 a, std::map<GLuint, MatInfo> matMap,
	std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<GLuint> texture_ids, std::vector<glm::vec2> texture_coords) :
		EntityGroup(p,s,a,matMap,vertices,normals,texture_ids,texture_coords)
{
	init();
}

void SpaceshipGroup::AddWeaponSystem(std::vector<Weapon> w, Entity* t, Scene* s) {
	for (int i = 0; i < w.size(); i++) {
		w.at(i).position += centre;
		weapons.push_back(w.at(i));
		//printf("\ncentre (%f,%f,%f)\n", centre.x, centre.y, centre.z);
		//printf("weapon pos (%f,%f,%f)\n", w.at(i).position.x, w.at(i).position.y, w.at(i).position.z);
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
			double offset_amplifier = 0.5;
			glm::dvec3 randomOffset((((rand() % 100) / 100.f)*weapon_target->size.x*offset_amplifier) - (weapon_target->size.x / 2.0),
				(((rand() % 100) / 100.f)*weapon_target->size.y*offset_amplifier) - (weapon_target->size.y / 2.0),
				(((rand() % 100) / 100.f)*weapon_target->size.z*offset_amplifier) - (weapon_target->size.z / 2.0));
			glm::dvec3 vel = (weapon_target->position + randomOffset) - glm::dvec3(weapons.at(w).position);

			Projectile projectile(weapon_target->position, weapons.at(w).position, glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), vel, { 1.f, 1.f, 0.1f });
			projectile.SetVelocity(vel, Entity::Dynamic);
			spawn_scene->AddProjectile(projectile);

			printf("fire\n");
		}
	}
}

void SpaceshipGroup::Update() {
	FireWeapons();
}