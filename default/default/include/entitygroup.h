#ifndef ENTITYGROUP_H
#define ENTITYGROUP_H

#include "Entity.h"
#include "Weapon.h"
#include "MatInfo.h"
#include <set>


class EntityGroup {
	void init();

public:
	int SHIELD_SPHERE_DETAIL = 200;
	int SHIELD_RIPPLE_DETAIL = 10;

	int STEP_SIZE = (SHIELD_SPHERE_DETAIL / 2) / SHIELD_RIPPLE_DETAIL;

	enum Type {Shield};

	// BFix
	std::vector<glm::vec3> original_pos, original_angle;
	glm::vec3 group_mov;
	glm::vec3 original_ang;
	glm::vec3 current_ang_mom, total_ang_mom;

	// --Animation
	glm::vec3 angle;
	// Sequence lists and details
	std::vector<GLuint> an_types, an_lengths;
	std::vector<GLfloat> an_ang_mags;
	std::vector<glm::vec3> an_ang_dirs;

	// Current status
	GLfloat mom_angular_magnitude;
	glm::vec3 mom_angular_direction, mom_movement;
	GLint frame, animation;

	// --Functions
	std::vector<Entity> en;
	glm::vec3 centre;

	// Constructors
	EntityGroup(Entity e);
	EntityGroup(glm::vec3 p, glm::vec3 s, glm::vec3 a, std::map<GLuint, MatInfo>,
		std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<GLuint> texture_ids, std::vector<glm::vec2> texture_coords);
	EntityGroup(Type t, glm::vec3 p, glm::vec3 s, glm::vec3 a); // shield

	// Update logic
	virtual void Update();
	virtual std::vector<Entity*> Render();

	// Manipulation of entities
	void Add(Entity e);
	void Move(GLint type, GLint length, glm::vec3 mag);
	void Rotate(GLint type, GLint length, GLfloat mag, glm::vec3 dir);
	void SetDefaultAngle(glm::vec3 direction);
	void SetAngle(glm::vec3 direction);

	// Animation
	void AddAnimation(GLuint type, GLuint length, glm::vec3 mag);
	void AddAnimation(GLuint type, GLuint length, GLfloat size, glm::vec3 dir);
	void Animate();
};

#endif /* ENTITYGROUP_H */