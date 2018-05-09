#ifndef SCENE_H
#define SCENE_H

#include <vector>

// OpenGL (GLFW/GLEW)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM (basics)
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "Vertex.h"
#include "entity.h"
#include "entitygroup.h"

class Scene {
	std::vector<Entity> en;

	glm::vec3 cameraAngle;
	glm::vec3 cameraRotation;
	float cameraSpeed = 0.01f;
	glm::vec3 cameraPosition;
	glm::vec3 background;


public:
	std::vector<Vertex> v;
	std::vector<EntityGroup> groups;

	Scene::Scene();
	Scene::Scene(glm::vec3 cam);

	void FreeGeometry();
	void SetupGeometry();

	void Rotate(glm::vec3 rot);

	void AddEntity(Entity e);
	void AddEntities(Entity* e, int n);
	void AddGroups(EntityGroup* e, int n);

	void UpdateEntityPosition(int index, glm::dvec3 pos);
	void UpdateEntityAngle(int index, glm::vec3 ang, float theta);
	
	void SetBackground(glm::vec3 background);

	void Update(GLint screenID);
	void Render(GLuint shaderprogram, GLuint vao);
	void DrawEntity(GLuint shaderprogram, glm::mat4 Projection, glm::mat4 View, Entity& e);
};

#endif /* SCENE_H */