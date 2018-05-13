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
#include "Projectile.h"
#include "ShieldGroup.h"

class Scene {
	std::vector<Entity> entities;
	std::vector<Projectile> pr;

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

	std::vector<btRigidBody*> bulletEntityBodies; // so that can get at all bits
	std::vector<btRigidBody*> bulletProjectileBodies; // so that can get at all bits
	std::vector<btRigidBody*> StaticBits; // especially during clean up.

public:
	std::vector<EntityGroup*> groups;

	Scene::Scene();
	Scene::Scene(glm::vec3 cam);

	void FreeGeometry();
	void SetupGeometry();

	void Rotate(glm::vec3 rot);

	void AddEntity(Entity e);
	void AddProjectile(Projectile e);
	void AddGroups(EntityGroup* e, int n);
	
	void SetBackground(glm::vec3 background);

	void Update(GLint screenID);
	void Render(GLuint shaderprogram, GLuint vao);
	void DrawEntity(GLuint shaderprogram, glm::mat4 Projection, glm::mat4 View, Entity& e);

	// Bullet
	void SetupPhysics();
	void UpdatePhysics(std::vector<btRigidBody*> bodies, std::vector<Entity>* objects);
	void UpdatePhysics(std::vector<btRigidBody*> bodies, std::vector<Projectile>* objects);
	void DestructPhysics();

	btRigidBody* SetSphere(float size, btTransform T, btVector3 velocity, Entity::Movement fixed);
	btRigidBody* SetCube(float size, btTransform T, btVector3 velocity);
	void setWall(btDiscreteDynamicsWorld* world, btVector3 side, double distanceFromCenter);

	void SpawnShield(glm::vec3 centre, glm::vec3 hitPoint);
};

#endif /* SCENE_H */