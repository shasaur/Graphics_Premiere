#include "scene.h"

Scene::Scene() {
	background = glm::vec3(0.f, 0.f, 0.f);
	cameraPosition = glm::vec3(0.f, 0.f, -5.0f);
	cameraRotation = glm::vec3(0.f, 0.f, 0.f);
	cameraAngle = glm::vec3(0.f, 0.f, 0.f);
	cameraSpeed = 0.f;

	groups = std::vector<EntityGroup>();
}

Scene::Scene(glm::vec3 cam) {
	background = glm::vec3(0.f, 0.f, 0.f);
	cameraPosition = cam;
	cameraRotation = glm::vec3(0.f, 0.f, 0.f);
	cameraAngle = glm::vec3(0.f, 0.f, 0.f);

	groups = std::vector<EntityGroup>();
}

void Scene::SetBackground(glm::vec3 b) {
	background = b;
}

void Scene::Rotate(glm::vec3 rot) {
	cameraRotation = rot;
}

void Scene::AddEntity(Entity e) {
	e.add_my_vertices(v);
	en.push_back(e);
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

void Scene::UpdateEntityPosition(int index, glm::dvec3 pos){
	en.at(index).position = pos;
}

void Scene::UpdateEntityAngle(int index, glm::vec3 ang, float theta){
	en.at(index).angle = ang;
	en.at(index).theta = theta;
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

void Scene::Update(GLint screenID) {
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

	for (int i = 0; i < groups.size(); i++) {
		groups.at(i).Animate();
	}
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
	glm::mat4 View = glm::mat4(1.f);

	cameraAngle[0] += cameraRotation.x;
	cameraAngle[1] += cameraRotation.y;
	cameraAngle[2] += cameraRotation.z;

	View = glm::translate(View, cameraPosition);
	View = glm::rotate(View, cameraAngle.x, glm::vec3(1.f, 0.f, 0.f));
	View = glm::rotate(View, cameraAngle.y, glm::vec3(0.f, 1.f, 0.f));
	View = glm::rotate(View, cameraAngle.z, glm::vec3(0.f, 0.f, 1.f));

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