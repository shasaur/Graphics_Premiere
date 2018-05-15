#include "entitygroup.h"

EntityGroup::EntityGroup(Entity e) {
	init();
	Add(e);
	centre = e.position;
}

EntityGroup::EntityGroup(glm::vec3 p, glm::vec3 s, glm::vec3 a, std::map<GLuint, MatInfo> matMap,
	std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<GLuint> texture_ids, std::vector<glm::vec2> texture_coords) {

	init();

	// Split this model into multiple entities, grouping by texture
	std::set<GLuint> texture_id_set(texture_ids.begin(), texture_ids.end());
	std::vector<GLuint> unique_texture_ids = std::vector<GLuint>();
	unique_texture_ids.assign(texture_id_set.begin(), texture_id_set.end());

	for (int j = 0; j < unique_texture_ids.size(); j++) {
		int index = unique_texture_ids.at(j);
		GLuint actual_texture_id = matMap.at(index).textId;

		std::vector<Vertex> entity_vertices;

		// Add every vertex that belong to a particular texture
		for (int i = 0; i < vertices.size(); i++) {
			if (texture_ids.at(i) == index) {
				Vertex vert;

				setPosition(vert, vertices.at(i).x, vertices.at(i).y, vertices.at(i).z); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
				setNormal(vert, vert.position[0], vert.position[1], vert.position[2]);
				setColour(vert, matMap.at(index).colour);
				vert.textureID = actual_texture_id;
				vert.uv = texture_coords.at(i);

				entity_vertices.push_back(vert);
			}
		}

		//Entity(Shape shape, glm::vec3 p, glm::vec3 s, glm::vec3 a, int res, GLuint textureID, glm::vec3 colour); // textured entity
		Entity e = Entity(Entity::Model, p, s, a, entity_vertices, actual_texture_id);
		e.SetupGeometry();
		en.push_back(e);
	}
}

// Pre-defined groups
EntityGroup::EntityGroup(Type t, glm::vec3 p, glm::vec3 s, glm::vec3 a) {
	init();

	switch (t) {
		case Shield: {

			int rows = (SHIELD_SPHERE_DETAIL / 4);

			bool stop = false;
			for (int i = 0; i < SHIELD_RIPPLE_DETAIL; i++) {
				int i0, i1;
				i0 = ((1.f / (float)(i + 1)) * 2.f) * ((float)rows/2.f);

				int j = i + 1;
				i1 = ((1.f / (float)(j + 1)) * 2.f) * ((float)rows / 2.f);

				if (i1 == i0)
					stop = true;

				if (stop)
					i0 = i1;

				//i1 = ((j+1) * STEP_SIZE) - ((STEP_SIZE * 10.f * ((float)(j - 1) / (float)SHIELD_RIPPLE_DETAIL)) - STEP_SIZE);

				Entity shieldTrial = Entity(Entity::Shield, p, s, a, SHIELD_SPHERE_DETAIL, rows-i0, rows-i1, glm::vec3(0.1f, 0.36f, 1.0f));
				centre = shieldTrial.position;
				Add(shieldTrial);
			}
			
		} break;
	}
}

void EntityGroup::init() {

	en = std::vector<Entity>();

	group_mov = glm::vec3(0.f, 0.f, 0.f);
	current_ang_mom = glm::vec3(0.f, 0.f, 0.f);
	total_ang_mom = glm::vec3(0.f, 0.f, 0.f);

	centre = glm::vec3(0.f, 0.f, 0.f);

	// Animation
	an_types = std::vector<GLuint>();
	an_lengths = std::vector<GLuint>();
	an_ang_mags = std::vector<GLfloat>();
	an_ang_dirs = std::vector<glm::vec3>();

	mom_angular_magnitude = 0.f;
	mom_angular_direction = glm::vec3(0.f, 0.f, 0.f);

	mom_movement = glm::vec3(0.f, 0.f, 0.f);


	frame = 0;
	animation = 0;

	angle = glm::vec3(0.f, 0.f, 0.f);
}

void EntityGroup::Add(Entity e) {
	e.SetupGeometry();
	en.push_back(e);
	original_pos.push_back(e.position);
	original_angle.push_back(e.angle);
}

void EntityGroup::Move(GLint type, GLint length, glm::vec3 mag) {
	AddAnimation(type, length, mag);
}

void EntityGroup::Rotate(GLint type, GLint length, GLfloat mag, glm::vec3 dir) {
	AddAnimation(type, length, mag, dir);
}

void EntityGroup::SetDefaultAngle(glm::vec3 direction) {
	original_ang = direction;
}

void EntityGroup::SetAngle(glm::vec3 direction) {
	glm::mat4 rotation = glm::mat4(1.f);
	rotation = glm::rotate(rotation, -original_angle.at(0).x, glm::vec3(1.f, 0.f, 0.f));
	rotation = glm::rotate(rotation, -original_angle.at(0).y, glm::vec3(0.f, 1.f, 0.f));
	rotation = glm::rotate(rotation, -original_angle.at(0).z, glm::vec3(0.f, 0.f, 1.f));

	rotation = glm::rotate(rotation, direction.x, glm::vec3(1.f, 0.f, 0.f));
	rotation = glm::rotate(rotation, direction.y, glm::vec3(0.f, 1.f, 0.f));
	rotation = glm::rotate(rotation, direction.z, glm::vec3(0.f, 0.f, 1.f));

	glm::vec3 direction_r = glm::vec3(direction.x, direction.z, direction.y);
	for (int i = 0; i < en.size(); i++) {
		en[i].position = original_pos.at(i) - centre;

		en[i].angle = direction_r;

		//glm::vec4 temp = rotation * glm::vec4(en[i].position, 1.f);
		glm::vec4 temp = rotation * glm::vec4(en[i].position, 1.f);// *glm::scale(glm::mat4(1.), en[i].size);
		en[i].position.x = temp.x;
		en[i].position.y = temp.y;
		en[i].position.z = temp.z;

		en[i].position += centre;

		// exp
		/*glm::mat4 r = glm::mat4(1.f);
		r = glm::rotate(r, angle.x, glm::vec3(1, 0, 0));
		r = glm::rotate(r, angle.y, glm::vec3(0, 1, 0));
		r = glm::rotate(r, angle.z, glm::vec3(0, 0, 1));

		glm::mat4 Model =
			glm::translate(glm::mat4(1.), glm::vec3(en[i].position)) *
			r *
			glm::scale(glm::mat4(1.), en[i].size);*/
	}

	//angle = direction;
	
}



/* This is for adding movement animations, the angle and the magnitude are in the same
variable, eg (1, 0, 1) with speed 2f is represented as (2, 0, 2) */
void EntityGroup::AddAnimation(GLuint type, GLuint length, glm::vec3 mag) {
	an_types.push_back(type);
	an_lengths.push_back(length);
	an_ang_dirs.push_back(mag);
	an_ang_mags.push_back(0.f);
}

/* For angular momentum, this is represented differently. The angle input
must be seperate to the direction input, otherwise we get incorrect angles
since sin(2pheta) =/= 2sin(pheta) */
void EntityGroup::AddAnimation(GLuint type, GLuint length, GLfloat size, glm::vec3 dir) {
	an_types.push_back(type);
	an_lengths.push_back(length);
	an_ang_dirs.push_back(dir);
	an_ang_mags.push_back(size);
}

void EntityGroup::Animate() {
	//for (int i = 0; i < en.size(); i++) {
		// If the entity is to be animated
		if (an_types.size() != 0) {

			// If not at the end of the animation sequence
			if (animation < an_types.size()) {

				// Take action depending on what the animation is supposed to do
				switch (an_types.at(animation)) {
				case 0:
					// do nothing
					break;
				case 1:
					mom_movement += an_ang_dirs.at(animation) * an_ang_mags.at(animation);
					break;
				case 2:
					mom_angular_magnitude = an_ang_mags.at(animation);
					mom_angular_direction = an_ang_dirs.at(animation);

					current_ang_mom += mom_angular_direction * mom_angular_magnitude;
					break;
				}

				group_mov += mom_movement;
				total_ang_mom += current_ang_mom;

				SetAngle(original_ang + total_ang_mom);
				for (int i = 0; i < en.size(); i++)
					en.at(i).position += group_mov;
				

				// Repeating and continuation mechanics
				frame++;

				if (frame > an_lengths.at(animation)) {
					animation++;
					frame = 0;
				}
			}
		}
	//}
}

void EntityGroup::Update() {

}

std::vector<Entity*> EntityGroup::Render() {
	std::vector<Entity*> entitiesToDraw = std::vector<Entity*>();

	for (int i = 0; i < en.size(); i++) {
		entitiesToDraw.push_back(&en.at(i));
	}

	return entitiesToDraw;
}