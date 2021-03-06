#include "scene.h"

Scene::Scene() {
	background = glm::vec3(0.f, 0.f, 0.f);
	cameraPosition = glm::vec3(0.f, -20.f, -20.0f);
	cameraRotation = glm::vec3(0.f, 0.0f, 0.f);
	cameraAngle = glm::vec3(0.f, 0.f, 0.f);
	cameraSpeed = 0.f;

	groups = std::vector<EntityGroup*>();

	frame = 0;
	SetupPhysics();  	// set up physics

	pr = std::vector<Projectile>();

	tour = std::vector<CameraAction>();
	onTour = false;
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

void Scene::AddEntity(Entity* e) {
	//e.add_my_vertices(v);

	e->SetupGeometry();
	entities.push_back(e);

	if (e->moving != Entity::None) {
		if (e->shape == Entity::Sphere)
			bulletEntityBodies.push_back(SetSphere(10.f, btTransform(btQuaternion(0, 0, 0, 1), btVector3(e->position.x, e->position.y, e->position.z)), e->vel, e->moving)); // last one is velocity
		else
			bulletEntityBodies.push_back(SetCube(e->size[0], btTransform(btQuaternion(0, 0, 0, 1), btVector3(e->position.x, e->position.y, e->position.z)), e->vel));
	}
	else
		bulletEntityBodies.push_back(nullptr);

}

void Scene::AddProjectile(Projectile e) {
	e.SetupGeometry();
	pr.push_back(e);

	if (e.shape == Entity::Sphere)
		bulletProjectileBodies.push_back(SetSphere(e.size[0], btTransform(btQuaternion(0, 0, 0, 1), btVector3(e.position.x, e.position.y, e.position.z)), e.vel, e.moving)); // last one is velocity
	else
		bulletProjectileBodies.push_back(SetCube(e.size[0], btTransform(btQuaternion(0, 0, 0, 1), btVector3(e.position.x, e.position.y, e.position.z)), e.vel));
}

//void Scene::AddEntities(Entity* e, int n) {
//	for (int i = 0; i < n; i++) {
//		e[i].add_my_vertices(v);
//		en.push_back(e[i]);
//	}
//}

void Scene::AddGroup(EntityGroup* eg) {
	groups.push_back(eg);
	for (int i = 0; i < eg->en.size(); i++) {
		Entity* e = &eg->en.at(i);
		if (e->moving != Entity::None) {
			//if (e->shape == Entity::Sphere)
			bulletEntityBodies.push_back(SetSphere(10.f, btTransform(btQuaternion(0, 0, 0, 1), btVector3(e->position.x, e->position.y, e->position.z)), e->vel, e->moving)); // last one is velocity
			//else
			//	bulletEntityBodies.push_back(SetCube(e->size[0], btTransform(btQuaternion(0, 0, 0, 1), btVector3(e->position.x, e->position.y, e->position.z)), e->vel));
		}
		else
			bulletEntityBodies.push_back(nullptr);

		entities.push_back(nullptr);
	}
}



void Scene::FreeGeometry() {
	for (int g = 0; g < groups.size(); g++) {
		for (int i = 0; i < groups.at(g)->en.size(); i++) {
			groups.at(g)->en.at(i).FreeGeometry();
		}
	}

	for (int i = 0; i < entities.size(); i++) {
		if (entities.at(i) != nullptr) {
			entities[i]->FreeGeometry();
		}
	}

	for (int i = 0; i < pr.size(); i++) {
		pr[i].FreeGeometry();
	}
}

void Scene::SetupGeometry() {
	for (int g = 0; g < groups.size(); g++) {
		for (int i = 0; i < groups.at(g)->en.size(); i++) {
			groups.at(g)->en.at(i).SetupGeometry();
		}
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

	//setWall(dynamicsWorld, btVector3(0, 1, 0), box_size);
	//setWall(dynamicsWorld, btVector3(1, 0, 0), box_size);
	//setWall(dynamicsWorld, btVector3(-1, 0, 0), box_size);
	//setWall(dynamicsWorld, btVector3(0, -1, 0), box_size);
	//setWall(dynamicsWorld, btVector3(0, 0, 1), box_size);
	//setWall(dynamicsWorld, btVector3(0, 0, -1), box_size);

	printf("Setup Bullet\n");
}

void Scene::UpdatePhysics(std::vector<btRigidBody*> bodies, std::vector<Entity*>* objects) {
	for (int i = 0; i < bodies.size(); i++) {
		if (bodies.at(i) != nullptr) {
			btTransform trans;
			btRigidBody* moveRigidBody = bodies[i];
			moveRigidBody->getMotionState()->getWorldTransform(trans);

			//printf("%f, %f, %f\n", position.x, position.y, position.z);
			glm::vec3 position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
			//btVector3 a = moveRigidBody->getAngularFactor();

			btQuaternion a = moveRigidBody->getCenterOfMassTransform().getRotation();
			glm::vec3 an = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
			float theta = moveRigidBody->getCenterOfMassTransform().getRotation().getAngle();

			//printf("%f, %f, %f\n", an.x, an.y, 0);
			//glm::mat4 btMat = glm::mat4(trans)
			//btMat.getRotation(trans.getRotation())

			if (objects->at(i) != nullptr) {
				objects->at(i)->position = position;
				objects->at(i)->angle = an;
				objects->at(i)->theta = theta;
			}
		}
	}
}

void Scene::UpdatePhysics(std::vector<btRigidBody*> bodies, std::vector<Projectile>* objects) {
	for (int i = 0; i < bodies.size(); i++) {
		if (bodies.at(i) != nullptr) {
			btTransform trans;
			btRigidBody* moveRigidBody;
			int n = bodies.size();
			moveRigidBody = bodies[i];
			moveRigidBody->getMotionState()->getWorldTransform(trans);

			//printf("%f, %f, %f\n", position.x, position.y, position.z);
			glm::vec3 position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
			//btVector3 a = moveRigidBody->getAngularFactor();

			btQuaternion a = moveRigidBody->getCenterOfMassTransform().getRotation();
			glm::vec3 an = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
			float theta = moveRigidBody->getCenterOfMassTransform().getRotation().getAngle();

			//printf("%f, %f, %f\n", an.x, an.y, 0);
			//glm::mat4 btMat = glm::mat4(trans)
			//btMat.getRotation(trans.getRotation())

			objects->at(i).position = position;
			objects->at(i).angle = an;
			objects->at(i).theta = theta;
		}
	}
}

void Scene::DestructPhysics() {
	/*
	* This is very minimal and relies on OS to tidy up.
	*/
	btRigidBody* moveRigidBody;
	moveRigidBody = bulletEntityBodies[0];
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

btRigidBody* Scene::SetSphere(float size, btTransform T, btVector3 velocity, Entity::Movement fixed) {
	btCollisionShape* fallshape = new btSphereShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = fixed == Entity::Dynamic ? 1 : 0;
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

float dot(glm::vec3 a, glm::vec3 b)  //calculates dot product of a and b
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float mag(glm::vec3 a)  //calculates magnitude of a
{
	return std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

void Scene::SpawnShield(glm::vec3 centre, glm::vec3 hitPoint) {

	float x = (hitPoint - centre).x;
	float y = (hitPoint - centre).y;
	float z = (hitPoint - centre).z;

	float ax = atan2(sqrt((y*y) + (z*z)), x);
	float ay = atan2(sqrt((z*z) + (x*x)), y);
	float az = atan2(sqrt((z*z) + (y*y)), z);

	printf("shield(%f,%f,%f)\n", ax,ay,az);

	// Shield pieces
	ShieldGroup* shield = new ShieldGroup(EntityGroup::Shield, centre, glm::vec3(10.f, 10.f, 10.f), glm::vec3(ax + (PI/2.0), ay, az));//centre - hitPoint);//glm::vec3(0.5f, -1.f, 2.f));
	groups.push_back(shield);
}


void Scene::Update(GLint screenID) {
	// Update physics
	dynamicsWorld->stepSimulation(1 / 120.f, 20);

	UpdatePhysics(bulletEntityBodies, &entities);
	UpdatePhysics(bulletProjectileBodies, &pr);

	for (int i = 0; i < pr.size(); i++) {
		btTransform trans;
		btRigidBody* moveRigidBody = bulletProjectileBodies[i];
		moveRigidBody->getMotionState()->getWorldTransform(trans);

		// Get current velocity of projectile
		btVector3 current_vel = moveRigidBody->getLinearVelocity();
		glm::vec3 current_vel_glm = glm::vec3(current_vel.getX(), current_vel.getY(), current_vel.getZ());


		if (pr.at(i).inTargetRange(current_vel_glm)) {
			printf("collision!");

			// Get current position of impact
			glm::vec3 position = glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());

			SpawnShield(pr.at(i).target_pos, position);

			// remove projectile, i--
			pr.erase(pr.begin() + i);
			bulletProjectileBodies.erase(bulletProjectileBodies.begin() + i);
			i--;
		}
	}

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
		//cameraPosition = glm::vec3(-10.f, 0.f, 0.0f);
		//cameraAngle = glm::vec3(0.f, -PI/2, 0.f);
	//}

	//cameraPosition.z += 0.02;
	//cameraAngle.y -= 0.001;

	for (int i = 0; i < groups.size(); i++) {
		groups.at(i)->Animate();
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
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "modelmatrix"), 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "viewmatrix"), 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(glGetUniformLocation(shaderprogram, "anglematrix"), 1, GL_FALSE, glm::value_ptr(Angle));
	glUniform3f(glGetUniformLocation(shaderprogram, "camerapos"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform1i(glGetUniformLocation(shaderprogram, "text"), 1);

	if (e.textured) {
		//printf("Drawing textured image\n");
		glUniform1i(glGetUniformLocation(shaderprogram, "text"), e.texID);
		glActiveTexture(GL_TEXTURE0 + e.texID);
		glBindTexture(GL_TEXTURE_2D, e.texID);
	}

	glDrawArrays(mode, 0, e.v.size());
	/* Invoke glDrawArrays telling that our data consists of a triangle fan */
	glBindVertexArray(0);

	if (e.textured) {
		glActiveTexture(GL_TEXTURE0 + e.texID);
		glBindTexture(GL_TEXTURE_2D, GL_TEXTURE0);
	}
}

glm::vec3 ConvertToCartesian(float* angles, float r) {
	float x = r * sin(angles[0]) * cos(angles[1]);
	float y = r * sin(angles[0]) * sin(angles[1]);
	float z = r * cos(angles[0]);
	return glm::vec3(x, y, z);
}

float* ConvertToPolar(glm::vec3 coords) {
	float x = coords.x;
	float y = coords.y;
	float z = coords.z;
	float r = sqrt(x * x + y * y + z * z);
	float* polars = new float[3]{ 
		r, 
		(x / sqrt(x * x + y * y)) * (y < 0 ? -1 : 1),
		acos(z / r) };
	return polars;
}

void Scene::UpdateCamera() {
	if (onTour) {

		if (tour.size() > 0) {
			CameraAction* currentAction = &tour.at(0);

			if (currentAction->currentTicks < currentAction->totalTicks) {
				switch (currentAction->type) {
				case Orbit: {
					if (currentAction->direction.x == -1.f) {

					}
					else {
						cameraAngle.x = currentAction->direction.x;
						cameraAngle.y = currentAction->direction.y;
						cameraAngle.z = currentAction->direction.z;
					}

					float* polars = new float[2]{
						currentAction->amount[1] * currentAction->currentTicks + currentAction->startPoint[0],
						currentAction->amount[2] * currentAction->currentTicks + currentAction->startPoint[1]
					};

					//float* newPolars = new float[2]{ polars[1],polars[2] };
					glm::vec3 newPos = ConvertToCartesian(polars, currentAction->amount[0]);
					cameraPosition.x = newPos.x - currentAction->target->position.x;
					cameraPosition.y = newPos.y - currentAction->target->position.y;
					cameraPosition.z = newPos.z - currentAction->target->position.z;

					currentAction->currentTicks += 1;

				} break;
				case Lerp: {
					if (currentAction->direction.x == -1.f) {

					}
					else {
						cameraAngle.x = currentAction->direction.x;
						cameraAngle.y = currentAction->direction.y;
						cameraAngle.z = currentAction->direction.z;
					}

					cameraPosition.x = (currentAction->amount[0] * currentAction->currentTicks) + currentAction->startPoint[0];
					cameraPosition.y = (currentAction->amount[1] * currentAction->currentTicks) + currentAction->startPoint[1];
					cameraPosition.z = (currentAction->amount[2] * currentAction->currentTicks) + currentAction->startPoint[2];

					currentAction->currentTicks += 1;

				} break;
				case Static: {

				}
				default:
					printf("Warning, no such camera action exists!");
				}
			}
			else {
				tour.at(0).currentTicks = 0;
				backupTour.push_back(tour.at(0));

				tour.erase(tour.begin());
			}
		}
		// If reached the end of the tour
		else {
			onTour = false;

			// reset queue
			tour = backupTour;
			backupTour = std::vector<CameraAction>();
		}
	}
}

void Scene::Render(GLuint shaderprogram, GLuint vao) {
	cameraAngle[0] += cameraRotation.x;
	cameraAngle[1] += cameraRotation.y;
	cameraAngle[2] += cameraRotation.z;

	UpdateCamera();

	GLfloat angle;
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 100000.0f);
	GLfloat t = glfwGetTime();
	GLfloat p = 400.f;
	t = fmod(t, p);
	angle = t * 360.f / p;
	//glm::mat4 View = glm::mat4(1.f);


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
		groups.at(g)->Update();
		std::vector<Entity*> entitiesToDraw = groups.at(g)->Render();
		
		// If this Shield group has ticked out, remove it
		if (entitiesToDraw.size() > 0 && entitiesToDraw.at(0) == nullptr) {
			groups.erase(groups.begin() + g);
			g--;
		}
		else {
			for (int i = 0; i < entitiesToDraw.size(); i++) {
				DrawEntity(shaderprogram, Projection, View, *entitiesToDraw.at(i));
			}
		}
	}

	for (int i = 0; i < entities.size(); i++) {
		if (entities.at(i) != nullptr) {
			DrawEntity(shaderprogram, Projection, View, *entities.at(i));
		}
	}

	for (int i = 0; i < pr.size(); i++) {
		DrawEntity(shaderprogram, Projection, View, pr.at(i));
	}
}