#include "scene.h"

Scene::Scene() {
	background = glm::vec3(0.f, 0.f, 0.f);
	cameraPosition = glm::vec3(0.f, 0.f, -20.0f);
	cameraRotation = glm::vec3(0.f, 0.0f, 0.f);
	cameraAngle = glm::vec3(0.f, 0.f, 0.f);
	cameraSpeed = 0.f;

	groups = std::vector<EntityGroup>();

	frame = 0;
	SetupPhysics();  	// set up physics
}

//Scene::Scene(glm::vec3 cam) {
//	background = glm::vec3(0.f, 0.f, 0.f);
//	cameraPosition = cam;
//	cameraRotation = glm::vec3(0.f, 0.02f, 0.f);
//	cameraAngle = glm::vec3(0.f, 0.f, 0.f);
//
//	groups = std::vector<EntityGroup>();
//
//	frame = 0; 
//	SetupPhysics();  	// set up physics
//}

void Scene::SetBackground(glm::vec3 b) {
	background = b;
}

void Scene::Rotate(glm::vec3 rot) {
	cameraRotation = rot;
}

void Scene::AddEntity(Entity e) {
	e.add_my_vertices(v);
	en.push_back(e);

	if (e.shape == Entity::Sphere)
		MovingBits.push_back(SetSphere(e.size[0], btTransform(btQuaternion(0, 0, 0, 1), btVector3(e.position.x, e.position.y, e.position.z)),e.vel)); // last one is velocity
	else
		MovingBits.push_back(SetCube(e.size[0], btTransform(btQuaternion(0, 0, 0, 1), btVector3(e.position.x, e.position.y, e.position.z)),e.vel));
}

void Scene::AddEntities(Entity* e, int n) {
	for (int i = 0; i < n; i++) {
		e[i].add_my_vertices(v);
		en.push_back(e[i]);
	}
}

void Scene::AddGroups(EntityGroup* e, int n) {
	for (int i = 0; i < n; i++) {
		groups.push_back(e[i]);
	}
}



void Scene::FreeGeometry() {
	for (int g = 0; g < groups.size(); g++) {
		for (int i = 0; i < groups.at(g).en.size(); i++) {
			groups.at(g).en.at(i).FreeGeometry();
		}
	}

	for (int i = 0; i < en.size(); i++) {
		en[i].FreeGeometry();
	}
}

void Scene::SetupGeometry() {
	for (int g = 0; g < groups.size(); g++) {
		for (int i = 0; i < groups.at(g).en.size(); i++) {
			groups.at(g).en.at(i).SetupGeometry();
		}
	}

	for (int i = 0; i < en.size(); i++) {
		en[i].SetupGeometry();
	}
}

void Scene::SetupPhysics() {
	/*
	* set up world
	*/
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0., GRAVITY, 0));

	double box_size = 3;

	setWall(dynamicsWorld, btVector3(0, 1, 0), box_size);
	setWall(dynamicsWorld, btVector3(1, 0, 0), box_size);
	setWall(dynamicsWorld, btVector3(-1, 0, 0), box_size);
	setWall(dynamicsWorld, btVector3(0, -1, 0), box_size);
	setWall(dynamicsWorld, btVector3(0, 0, 1), box_size);
	setWall(dynamicsWorld, btVector3(0, 0, -1), box_size);

	printf("Setup Bullet ");
	int n = MovingBits.size();
	printf("%d", n);
}

void Scene::UpdatePhysics() {
	for (int i = 0; i < MovingBits.size(); i++) {
		btTransform trans;
		btRigidBody* moveRigidBody;
		int n = MovingBits.size();
		moveRigidBody = MovingBits[i];
		moveRigidBody->getMotionState()->getWorldTransform(trans);

		//printf("%f, %f, %f\n", position.x, position.y, position.z);
		glm::vec3 position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		//btVector3 a = moveRigidBody->getAngularFactor();

		btQuaternion a = moveRigidBody->getCenterOfMassTransform().getRotation();
		glm::vec3 an = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
		float theta = moveRigidBody->getCenterOfMassTransform().getRotation().getAngle();

		printf("%f, %f, %f\n", an.x, an.y, 0);
		//glm::mat4 btMat = glm::mat4(trans)
		//btMat.getRotation(trans.getRotation())

		en.at(i).position = position;
		en.at(i).angle = an;
		en.at(i).theta = theta;
	}
}

void Scene::DestructPhysics() {
	/*
	* This is very minimal and relies on OS to tidy up.
	*/
	btRigidBody* moveRigidBody;
	moveRigidBody = MovingBits[0];
	dynamicsWorld->removeRigidBody(moveRigidBody);
	delete moveRigidBody->getMotionState();
	delete moveRigidBody;
	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;
}


// BULLET PHYSICS START //

btRigidBody* Scene::SetSphere(float size, btTransform T, btVector3 velocity) {
	btCollisionShape* fallshape = new btSphereShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(velocity);
	fallRigidBody->setRestitution(COE);
	fallRigidBody->setFriction(0.0f);
	fallRigidBody->setRollingFriction(0.0f);
	fallRigidBody->setDamping(0.0f, 0.0f);
	dynamicsWorld->addRigidBody(fallRigidBody);

	return fallRigidBody;
}

btRigidBody* Scene::SetCube(float size, btTransform T, btVector3 velocity) {
	btCollisionShape* fallshape = new btBoxShape(btVector3(size, size, size));
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(velocity);
	fallRigidBody->setRestitution(COE);
	fallRigidBody->setFriction(0.0f);
	fallRigidBody->setRollingFriction(0.0f);
	fallRigidBody->setDamping(0.0f, 0.0f);
	dynamicsWorld->addRigidBody(fallRigidBody);

	return fallRigidBody;
}

void Scene::setWall(btDiscreteDynamicsWorld* world, btVector3 side, double distanceFromCenter) {
	btCollisionShape* shape = new btStaticPlaneShape(side, 0);
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1),
		btVector3(side.getX()*distanceFromCenter*-1, side.getY()*distanceFromCenter*-1, side.getZ()*distanceFromCenter*-1)));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, shape, btVector3(0, 0, 0));
	btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
	rigidBody->setRestitution(COE);
	world->addRigidBody(rigidBody);
}


// BULLET PHYSICS END //





void Scene::Update(GLint screenID) {
	// Update physics
	dynamicsWorld->stepSimulation(1 / 120.f, 20);

	UpdatePhysics();

	//for (int i = 0; i < en.size(); i++) {
	//	if (screenID == 2) {
	//		if (i == 0 || i == 3 || i == 7) {
	//			en.at(i).Animate(en.at(3).position/*-(glm::dvec3(en.at(3).size/2.f))*/);
	//		}
	//		else if (i == 1 || i == 4 || i == 9) {
	//			en.at(i).Animate(en.at(4).position);
	//		}
	//		else {
	//			en.at(i).Animate(glm::vec3(0.f,0.f,0.f));
	//		}
	//	}
	//}

	//if (frame % 600 <= 300) {
	//	cameraPosition = glm::vec3(0.f, 0.f, -10.0f);
	//	cameraAngle = glm::vec3(0.f, 0.f, 0.f);
	//} else {
		cameraPosition = glm::vec3(-10.f, 0.f, 0.0f);
		cameraAngle = glm::vec3(0.f, -PI/2, 0.f);
	//}

	//cameraPosition.z += 0.02;
	//cameraAngle.z += 0.01;

	for (int i = 0; i < groups.size(); i++) {
		groups.at(i).Animate();
	}

	frame++;
}

void Scene::DrawEntity(GLuint shaderprogram, glm::mat4 Projection, glm::mat4 View, Entity& e) {
	glBindVertexArray(e.vao);

	glm::mat4 Model = e.model_transform();
	glm::mat4 Angle = e.angle_matrix();

	//glm::mat4 Model = glm::mat4(1.f);
	//Model = glm::translate(Model, en.at(i).position);
	//Model = glm::rotate(Model, en.at(i).angle);
	glm::mat4 MVP = Projection * View * Model;

	GLenum mode = NULL;

	if (e.wiremesh)
		mode = GL_LINE_STRIP;

	else
		mode = GL_TRIANGLES;


	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "mvpmatrix"), 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelmatrix"), 1, GL_FALSE, glm::value_ptr(Angle));

	if (e.textured) {
		glUniform1i(glGetUniformLocation(shaderprogram, "text"), e.texID);
		glActiveTexture(GL_TEXTURE0 + e.texID);
		glBindTexture(GL_TEXTURE_2D, e.texID);
	}

	glDrawArrays(mode, 0, e.v.size());
	/* Invoke glDrawArrays telling that our data consists of a triangle fan */
	glBindVertexArray(0);
}

void Scene::Render(GLuint shaderprogram, GLuint vao) {
	GLfloat angle;
	glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
	GLfloat t = glfwGetTime();
	GLfloat p = 400.f;
	t = fmod(t, p);
	angle = t * 360.f / p;
	//glm::mat4 View = glm::mat4(1.f);

	cameraAngle[0] += cameraRotation.x;
	cameraAngle[1] += cameraRotation.y;
	cameraAngle[2] += cameraRotation.z;

	//View = glm::translate(View, cameraPosition);
	//View = glm::rotate(View, cameraAngle.x, glm::vec3(1.f, 0.f, 0.f));
	//View = glm::rotate(View, cameraAngle.y, glm::vec3(0.f, 1.f, 0.f));
	//View = glm::rotate(View, cameraAngle.z, glm::vec3(0.f, 0.f, 1.f));


	glm::mat4 r = glm::mat4(1.f);
	r = glm::rotate(r, cameraAngle.x, glm::vec3(1, 0, 0));
	r = glm::rotate(r, cameraAngle.y, glm::vec3(0, 1, 0));
	r = glm::rotate(r, cameraAngle.z, glm::vec3(0, 0, 1));

	glm::mat4 View = r * glm::translate(glm::mat4(1.), cameraPosition);// *r;


	/* Bind our modelmatrix variable to be a uniform called mvpmatrix in our shaderprogram */
	glClearColor(background.r, background.g, background.b, 1.0f);  /* Make our background black */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int g = 0; g < groups.size(); g++) {
		for (int i = 0; i < groups.at(g).en.size(); i++) {
			DrawEntity(shaderprogram, Projection, View, groups.at(g).en.at(i));
		}
	}

	for (int i = 0; i < en.size(); i++) {
		DrawEntity(shaderprogram, Projection, View, en.at(i));
	}
}