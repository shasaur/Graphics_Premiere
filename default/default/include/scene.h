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

	int frame;

	// Bullet physics
	const double PI = 3.141592653589793;
	const double PIo2 = PI / 2.;
	const double PIo4 = PI / 4.;
	const double PI2 = PI * 2.;
	const float lod = PI / 32.;

	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<btRigidBody*> MovingBits; // so that can get at all bits
	std::vector<btRigidBody*> StaticBits; // especially during clean up.

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
	
	void SetBackground(glm::vec3 background);

	void Update(GLint screenID);
	void Render(GLuint shaderprogram, GLuint vao);
	void DrawEntity(GLuint shaderprogram, glm::mat4 Projection, glm::mat4 View, Entity& e);

	// Bullet
	void Scene::SetupPhysics();
	void Scene::UpdatePhysics();
	void Scene::DestructPhysics();

	btRigidBody* Scene::SetSphere(float size, btTransform T, btVector3 velocity);
	btRigidBody* Scene::SetCube(float size, btTransform T, btVector3 velocity);
	void Scene::setWall(btDiscreteDynamicsWorld* world, btVector3 side, double distanceFromCenter);
};

#endif /* SCENE_H */