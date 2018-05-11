#include "entity.h"

Entity::Entity(Shape sh) {
	position = glm::vec3(0.f, 0.f, 0.f);
	size = glm::vec3(1.f, 1.f, 1.f);
	angle = glm::vec3(1.f, 1.f, 1.f);
	shape = sh;

	wiremesh = false;
	textured = false;

	switch (sh) {
	case Shape::Cone:
	{break; };
	case Shape::Cube:
	{break; };
	case Shape::Cylinder:{
		break; };
	case Shape::Sphere:{
		CreateSimpleSphere(10, false);
		break;};
	}
}

// Model constructor
Entity::Entity(glm::vec3 p, glm::vec3 s, glm::vec3 a, 
		std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<GLuint> texture_ids, std::vector<glm::vec2> texture_coords,
		GLuint texture_group) {
	position = p;
	size = s;
	angle = a;

	wiremesh = false;
	textured = true;

	for (int i = 0; i < vertices.size(); i++) {
		if (texture_ids.at(i) == texture_group) {
			Vertex vert;

			setPosition(vert, vertices.at(i).x, vertices.at(i).y, vertices.at(i).z); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
			setNormal(vert, vert.position[0], vert.position[1], vert.position[2]);
			setColour(vert, glm::vec3(1.f, 1.f, 1.f));
			vert.textureID = texture_ids.at(i);
			vert.uv = texture_coords.at(i);

			push(vert);
		}
	}
}

Entity::Entity(glm::vec3 p, glm::vec3 s, glm::vec3 a, std::vector<Vertex> vertices, GLuint textureID) {
	position = p;
	size = s;
	angle = a;

	wiremesh = false;
	textured = true;
	texID = textureID;

	for (int i = 0; i < vertices.size(); i++) {
		push(vertices.at(i));
	}
}

Entity::Entity(Shape sh, glm::vec3 p, glm::vec3 s, glm::vec3 a, int res, bool w, glm::vec3 colour) {
	position = p;
	size = s;
	angle = a;
	shape_colour = colour;
	shape = sh;

	wiremesh = w;
	textured = false;

	switch (sh) {
	case Shape::Cone:{
		CreateCone(glm::vec3(0.f, 0.f, 0.f), 2.f, { 0.f, 0.f, 1.f });
		break; };
	case Shape::Cube: {
		CreateCube(w);
		break; };
	case Shape::Cylinder:{
		CreateCylinder(true);
		break; };
	case Shape::Sphere:{
		CreateSimpleSphere(res, w);
		break; };
	}
}


Entity::Entity(Shape shape, glm::vec3 p, glm::vec3 s, glm::vec3 a, int res, bool w, GLuint textureID, glm::vec3 colour) {
	position = p;
	size = s;
	angle = a;
	shape_colour = colour;

	wiremesh = w;
	textured = true;
	texID = textureID;

	switch (shape) {
	case Shape::Cone: {
		CreateCone(glm::vec3(0.f, 0.f, 0.f), 2.f, { 0.f, 0.f, 1.f });
		break; };
	case Shape::Cube: {
		CreateCube(w);
		break; };
	case Shape::Cylinder: {
		CreateCylinder(true);
		break; };
	case Shape::Sphere: {
		CreateSimpleSphere(res, w);
		break; };
	}
}

void Entity::SetVelocity(btVector3 newVel) {
	vel = newVel;
}

glm::vec2 getPolar(glm::vec3 v)
{
	float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return glm::vec2(atan(v.y / v.x), acos(v.z / r));
}

void Entity::FreeGeometry() {
	glDeleteBuffers(1, &vbo);
}

void Entity::SetupGeometry() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate an identifier to use for the Vertex Buffer Object, store it in vbo
	glGenBuffers(1, &vbo);
	// Make this identifier the active one (storing vertex attributes)
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// Give vertex data in v to the Vertex Buffer Object in OpenGL
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(struct Vertex), v.data(), GL_STATIC_DRAW);

	// usual setting up of memory locations to cycle through for attributes of vertices during rendering
	glEnableVertexAttribArray(0);
	glVertexAttribPointer((GLuint)0, // coordinate data will be in attribute index 0
		3, GL_FLOAT,	// use 3 decimals to represent a vertex
		GL_FALSE,		// not normalised
		sizeof(struct Vertex),	// stride (aka memory to jump to get to the next vertex)
		(const GLvoid*)offsetof(struct Vertex, position));	// coordinates are stored in the vertex.pos space

															// colour data will be in attribute index 1, and the remaining characteristics to read the data
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(struct Vertex, color));   // bug );
	glEnableVertexAttribArray(1);

	// normal data will be in attribute index 2
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(struct Vertex, normal));   // bug );
	glEnableVertexAttribArray(2);

	if (textured) {
		//glActiveTexture(GL_TEXTURE0 + texID);
		//glBindTexture(GL_TEXTURE_2D, texID);

		// angle data to fix the light after the object has been rotated, so that it diffuses the right way
		glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(struct Vertex, uv));   // bug );
		glEnableVertexAttribArray(3);

		//glVertexAttribPointer((GLuint)4, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (const GLvoid*)offsetof(struct Vertex, uv));
		//glEnableVertexAttribArray(4);
	}

	glBindVertexArray(0);
}

/* Note:
	Because the task is to make the objects be illuminated by light at infinity, they are not off-centered
	by their distance in the lighting calculation. This will therefore make all entities render lights
	in parallel, much like the light rays potray when coming from infinity. */

//void Entity::CreateModel() {
//	std::vector<tinyobj::shape_t> shapes;
//	std::vector<tinyobj::material_t> materials;
//	std::vector< glm::vec3 > vertices;
//	std::vector< Vertex > o;
//
//	std::string obj_err =
//		tinyobj::LoadObj(shapes, materials, filename, NULL);
//
//	for (int i = 0; i < shapes.size(); i++)
//		for (int j = 0; j < shapes[i].mesh.indices.size(); j++)
//			vertices.push_back(glm::vec3(
//				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3],
//				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 1],
//				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 2]
//			));
//}
	
void Entity::CreateCube(bool wiremesh){
	//float d = 0.5f;
	
	std::vector<std::vector<Vertex>> faces = std::vector<std::vector<Vertex>>(); 
	
	// Go through each pair of faces
	int d[] = {0,0,0};
	
	for (int i=0; i<3; i++){
		for (d[0]=-1; d[0]<2; d[0]+=2){
			std::vector<Vertex> part1 = std::vector<Vertex>();
			std::vector<Vertex> part2 = std::vector<Vertex>();
			
			for (d[1]=-1; d[1]<2; d[1]+=2){
				for (d[2]=-1; d[2]<2; d[2]+=2){
					Vertex v1;
					setColour(v1, shape_colour); 
					setPosition(v1, d[(i+0)%3], d[(i+1)%3], d[(i+2)%3]);
					setNormal(v1, v1.position[0], v1.position[1], v1.position[2]);
					v1.uv = glm::vec2((atan2(v1.position[0], v1.position[2]) / 3.1415926f + 1.0f) * 0.5, (asin(v1.position[1]) / 3.1415926 + 0.5));
					
					if (part1.size() < 3)
						part1.push_back(v1);
					else if (part1.size() == 3 && part2.size() == 0) {
						part2.push_back(part1.at(1));
						part2.push_back(part1.at(2));
						part2.push_back(v1);
					}
				}
			}
			
			push(part1);
			push(part2);
		}
	}
}

void Entity::CreateSimpleSphere(int n, bool wiremesh)
{
	int i, j;
	double theta1, theta2, theta3;
	glm::vec3 e, p;

	GLfloat r = 1.f;
	// vertical
	for (j = 0; j<n / 2; j++) {
		theta1 = j * TWOPI / n - PID2;
		theta2 = (j + 1) * TWOPI / n - PID2;

		// horizontal
		for (i = 0; i <= n; i++) {

			theta3 = i * TWOPI / n;

			// SIMPLIFY ALL OF THIS

			Vertex v1;
			setColour(v1, shape_colour); 
			setPosition(v1, r * cos(theta2) * cos(theta3), sin(theta2), r * cos(theta2) * sin(theta3));
			setNormal(v1, v1.position[0], v1.position[1], v1.position[2]); 
			v1.uv = glm::vec2((atan2(v1.position[0], v1.position[2]) / 3.1415926f + 1.0f) * 0.5, (asin(v1.position[1]) / 3.1415926 + 0.5));

			e.x = cos(theta1) * cos(theta3);
			e.y = sin(theta1);
			e.z = cos(theta1) * sin(theta3);

			Vertex v2;
			setColour(v2, v1);
			setPosition(v2, r * e.x, r * e.y, r * e.z);
			setNormal(v2, v2.position[0], v2.position[1], v2.position[2]);
			v2.uv = glm::vec2((atan2(v2.position[0], v2.position[2]) / 3.1415926f + 1.0f) * 0.5, (asin(v2.position[1]) / 3.1415926 + 0.5));


			theta3 = (i + 1) * TWOPI / n;

			e.x = cos(theta2) * cos(theta3);
			e.y = sin(theta2);
			e.z = cos(theta2) * sin(theta3);

			Vertex v3;
			setColour(v3, v1);
			setPosition(v3, r * e.x, r * e.y, r * e.z);
			setNormal(v3, v3.position[0], v3.position[1], v3.position[2]);
			v3.uv = glm::vec2((atan2(v3.position[0], v3.position[2]) / 3.1415926f + 1.0f) * 0.5, (asin(v3.position[1]) / 3.1415926 + 0.5));


			e.x = cos(theta1) * cos(theta3);
			e.y = sin(theta1);
			e.z = cos(theta1) * sin(theta3);

			Vertex v4;
			setColour(v4, v1);
			setPosition(v4, r * e.x, r * e.y, r * e.z);
			setNormal(v4, v4.position[0], v4.position[1], v4.position[2]);
			v4.uv = glm::vec2((atan2(v4.position[0], v4.position[2]) / 3.1415926f + 1.0f) * 0.5, (asin(v4.position[1]) / 3.1415926 + 0.5));

			if (wiremesh) {
				push(v1);
				push(v2);
				push(v4);
				push(v3);
			}
			else {
				push(v1);
				push(v2);
				push(v3);

				push(v2);
				push(v3);
				push(v4);
			}
		}
	}
}

void Entity::CreateCylinder(bool capped) {
	GLfloat cf = 0.0;/*
	Vertex top_v;
	top_v.position[0] = 0;
	top_v.position[1] = 0;
	top_v.position[2] = 0;
	top_v.color[0] = cf;
	cf = 1. - cf;
	top_v.color[1] = cf;
	cf = 1. - cf;
	top_v.color[2] = cf;
	cf = 1. - cf;*/


	GLint lod = 32;
	GLfloat step = 2.f * 3.141596f / GLfloat(lod);
	GLfloat Radius = 1.f;
	for (GLfloat a = 0; a <= (2.f * 3.141596f + step); a += step) {

		Vertex v1;
		setPosition(v1, Radius * cos(a), Radius * sin(a), -1.f); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
		setNormal(v1, v1.position[0], v1.position[1], v1.position[2]);
		setColour(v1, shape_colour);

		Vertex v2;
		setPosition(v2, Radius * cos(a + step), Radius * sin(a + step), -1.f); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
		setNormal(v2, v2.position[0], v2.position[1], v2.position[2]);
		setColour(v2, shape_colour);

		Vertex v3;
		setPosition(v3, Radius * cos(a), Radius * sin(a), 1.f); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
		setNormal(v3, v3.position[0], v3.position[1], v3.position[2]);
		setColour(v3, v1);

		Vertex v4;
		setPosition(v4, Radius * cos(a + step), Radius * sin(a + step), 1.f); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
		setNormal(v4, v4.position[0], v4.position[1], v4.position[2]);
		setColour(v4, v2);
/*
		Print(v1.position);
		Print(v2.position);
		Print(v3.position);
		Print(v4.position);*/

		// Lower left triangle
		push(v1);
		push(v2);
		push(v3);

		// Upper right triangle
		push(v2);
		push(v3);
		push(v4);
	}

	// If the container should have 'lids'
	if (capped) {
		glm::vec3 c(0.f, 0.f, -1.f);
		glm::vec3 normalDirection2 = { 0.f, 0.f, -1.f };
		CreateCone(c, 0.f, normalDirection2);

		c[2] = 1.f;
		glm::vec3 normalDirection = { 0.f, 0.f, 1.f };
		CreateCone(c, 0.f, normalDirection);
	}
}

void Entity::CreateCone(glm::vec3 c, GLfloat height, glm::vec3 n) {

	GLfloat cf = 0.0;
	Vertex top_v;
	setPosition(top_v, c[0], c[1], c[2]);
	if (height != 0)
		setNormal(top_v, top_v.position[0] - c[0], top_v.position[1] - c[1], top_v.position[2] - c[2]);
	else
		setNormal(top_v, n[0], n[1], n[2]);

	//setColour(top_v, 0.5f, 0.5f, 0.5f);

	setColour(top_v, shape_colour);


	GLint lod = 32;
	GLfloat step = 2.f * 3.141596f / GLfloat(lod);
	GLfloat Radius = 1.f;
	for (GLfloat a = 0; a <= (2.f * 3.141596f + step); a += step) {

		Vertex v1;
		setPosition(v1, Radius * cos(a) + c[0], Radius * sin(a) + c[1], height + c[2]); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
		if (height != 0)
			setNormal(v1, v1.position[0] - c[0], v1.position[1] - c[1], v1.position[2] - c[2]);
		else
			setNormal(v1, n[0], n[1], n[2]);

		//setColour(v1, 0.5f, 0.5f, 0.5f);
		setColour(v1, top_v);

		Vertex v2;
		setPosition(v2, Radius * cos(a + step) + c[0], Radius * sin(a + step) + c[1], height + c[2]); //vertex: width x height x length (set to 0.0 for a circle (flat), >= 1.0 for a cone)
		if (height != 0)
			setNormal(v2, v2.position[0] - c[0], v2.position[1] - c[1], v2.position[2] - c[2]);
		else
			setNormal(v2, n[0], n[1], n[2]);


		//setColour(v2, 0.5f, 0.5f, 0.5f);
		setColour(v1, top_v);

		// Triangle
		push(v1);
		push(v2);
		push(top_v); // Apex
	}
}

glm::mat4 Entity::angle_matrix() {
	glm::mat4 r = glm::mat4(1.f);
	r = glm::rotate(r, angle.x, glm::vec3(1, 0, 0));
	r = glm::rotate(r, angle.y, glm::vec3(0, 1, 0));
	r = glm::rotate(r, angle.z, glm::vec3(0, 0, 1));
	return r;
}

glm::mat4 Entity::model_transform() {
	glm::mat4 r = glm::mat4(1.f);
	r = glm::rotate(r, angle.x, glm::vec3(1, 0, 0));
	r = glm::rotate(r, angle.y, glm::vec3(0, 1, 0));
	r = glm::rotate(r, angle.z, glm::vec3(0, 0, 1));
	
	glm::mat4 Model =
		glm::translate(glm::mat4(1.), glm::vec3(position)) *
		r *
		glm::scale(glm::mat4(1.), size);


	return Model;
}

void Entity::add_my_vertices(std::vector<Vertex> &vertices) {
	for (int i = 0; i < v.size(); i++)
		vertices.push_back(v.at(i));
}

// Pushes and normalises the vertex such that lighting is correctly displayed in the new angle
void Entity::push(Vertex &vertex) {
	vertex.angle[0] = angle.x;
	vertex.angle[1] = angle.y;
	vertex.angle[2] = angle.z;
	v.push_back(vertex);
}

void Entity::push(std::vector<Vertex> &vertex) {
	for (int i=0; i<vertex.size(); i++){
		vertex[i].angle[0] = angle.x;
		vertex[i].angle[1] = angle.y;
		vertex[i].angle[2] = angle.z;
		v.push_back(vertex[i]);
	}
}

glm::mat4 GetRotationMatrix(glm::vec3 my_angle) {
	glm::mat4 t = glm::mat4(1.f);

	t = glm::rotate(t, my_angle.x, glm::vec3(1, 0, 0));
	t = glm::rotate(t, my_angle.y, glm::vec3(0, 1, 0));
	t = glm::rotate(t, my_angle.z, glm::vec3(0, 0, 1));

	return t;
}