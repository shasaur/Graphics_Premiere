#ifndef ENTITY_H
#define ENTITY_H

#include <stdio.h>

// OpenGL (GLFW/GLEW)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM (basics)
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Bullet Physics (basics)
#include <btBulletDynamicsCommon.h>

#include "World.h"

#include "glslprogram.h"
#include "utils.h"

#include "Vertex.h"
#include <vector>


// Sphere-related constants
#define TWOPI           6.283185307179586476925287
#define PID2            1.570796326794896619231322

class Entity {
	void push(Vertex &vertex);
	void push(std::vector<Vertex> &vertex);

public:
	GLuint vao, vbo;
	GLuint texID;

	void FreeGeometry();
	void SetupGeometry();

	enum Shape { Sphere, Cone, Cylinder, Cube, Shield };

	glm::dvec3 position;
	glm::vec3 size;
	btVector3 vel;

	glm::vec3 angle;
	float theta;

	glm::vec3 shape_colour;

	bool wiremesh, textured;

	Shape shape;

	std::vector<Vertex> v;

	// - Functions -
	Entity(Shape shape);
	Entity(glm::vec3 p, glm::vec3 s, glm::vec3 a, std::vector<Vertex> vertices, GLuint textureID);
	Entity(Shape shape, glm::vec3 p, glm::vec3 s, glm::vec3 a, int res, bool wiremesh, glm::vec3 colour = { -1.f,-1.f,-1.f }); // solid colour entity
	Entity(Shape shape, glm::vec3 p, glm::vec3 s, glm::vec3 a, int res, bool wiremesh, GLuint textureID, glm::vec3 colour = { -1.f,-1.f,-1.f }); // textured entity
	Entity(glm::vec3 p, glm::vec3 s, glm::vec3 a,
		std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<GLuint> texture_ids, std::vector<glm::vec2> texture_coords,
		GLuint texture_group); // model loading

	void CreateCube(bool wiremesh);
	void CreateShieldSphere(int n, bool wiremesh);
	void CreateSimpleSphere(int n, bool wiremesh);
	void CreateCylinder(bool capped);
	void CreateCone(glm::vec3 c, GLfloat height, glm::vec3 n);

	void add_my_vertices(std::vector<Vertex> &vertices);

	glm::mat4 angle_matrix();
	glm::mat4 model_transform();


	void Entity::SetVelocity(btVector3 vel);
	void MoveModel(glm::dvec3 p);
};


#endif /* ENTITY_H */