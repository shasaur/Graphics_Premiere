#include "main.h"

int renderMode = 1;
bool pause = false;

/* This is a handle to the shader program */
GLuint s1program, s2program, s3program;
GLuint vao, vbo[1]; /* Create handles for our Vertex Array Object and One Vertex Buffer Object */

std::vector<Vertex> v;

Scene* scenes[4];
int current_scene = 0;

float speed = 0.1f;

GLuint default_texture, sun_texture, mars_texture, saturn_texture, titan_texture, no_texture, space_texture;
//std::map<std::string, GLuint> texture_map;

///*
//Create a simple sphere
//"method" is 0 for quads, 1 for triangles
//(quads look nicer in wireframe mode)/
//*/
//typedef struct {
//	double x, y, z;
//} XYZ;

GLuint loadTexture(const char *fname) {
	int w, h, n;
	unsigned char *data = stbi_load(fname, &w, &h, &n, 0);
	if (data == NULL) {
		fprintf(stderr, "Image not loaded: %s", fname);
		const char *error = stbi_failure_reason();
		fprintf(stderr, "Failure reason %s\n", error);
		return NULL;
	}

	GLuint tex = 1;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	delete data;
	return tex;
}

// loads an image into a gl texture
GLuint load_texture_blank()
{
	int w = 1, h = 1;
	unsigned char *data = new unsigned char[3 * w * h];
	for (int i = 0; i < 3 * w*h; ++i)
		data[i] = 0;

	GLuint tex = 1;
	glGenTextures(1, &tex);
	//glBindTexture(GL_TEXTURE_2D, tex);
	// set the texture wrapping parameters
	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete data;
	return tex;
}

float rnd(float limit, float offset) {
	return ((float)(rand() % 100) / (100.f / limit)) - offset;
}

//void AddPhysicalObject(Scene &scene, Entity::Shape shape, float size, glm::vec3 pos, glm::dvec3 velocity);

//void AddPhysicalObject(Scene &scene, Entity::Shape shape, float size) {
//	AddPhysicalObject(scene, shape, size,
//		glm::vec3(rnd(20, 10), rnd(20, 10), rnd(20, 10)),
//		glm::vec3(rnd(6, 3), rnd(6, 3), rnd(6, 3)));
//}

//void AddPhysicalObject(Scene &scene, Entity::Shape shape, float size, glm::vec3 pos, glm::dvec3 velocity) {
//	//Entity e1(Entity::Sphere, glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.f, 0.f, 0.f), 40, true);
//	
//
//	
//}

EntityGroup* loadModelAt(std::string base, std::string objFile, glm::vec3 p, glm::vec3 s, glm::vec3 a, bool spaceship) {
	std::vector<tinyobj::shape_t> shapes = std::vector<tinyobj::shape_t>();
	std::vector<tinyobj::material_t> materials = std::vector<tinyobj::material_t>();

	std::map<GLuint, MatInfo> materialMap = std::map<GLuint, MatInfo>();

	std::string obj_err =
		tinyobj::LoadObj(shapes, materials, objFile.c_str(), base.c_str());

	// Go through each material, map its info, and load its texture
	for (int i = 0; i < materials.size(); i++) {
		std::string name = materials[i].name;
		float* colour = materials[i].diffuse;

//printf("material[%d].diffuse_texname = %s\n", i, materials[i].diffuse_texname.c_str());

		// Load texture if one exists
		if (materials[i].diffuse_texname != "") {
			std::string tex_name = materials[i].diffuse_texname;
			GLuint textId = loadTexture(std::string(base).append(tex_name).c_str());
//texture_map.insert(std::make_pair(materials[i].diffuse_texname.c_str(), t));

			materialMap.insert(std::make_pair(i, MatInfo(glm::vec3(colour[0], colour[1], colour[2]), textId, name.c_str(), tex_name.c_str())));
		}
		else {
			materialMap.insert(std::make_pair(i, MatInfo(glm::vec3(colour[0], colour[1], colour[2]), 1, name.c_str(), "")));
		}
		

		//if (materialMap.find(i) == materialMap.end())
	}

	//materials.push_back(tinyobj::material_t());

	std::vector<glm::vec3> vertices = std::vector<glm::vec3>();
	std::vector<glm::vec3> colours = std::vector<glm::vec3>();
	std::vector<glm::vec3> normals = std::vector<glm::vec3>();
	std::vector<GLuint> texture_ids = std::vector<GLuint>();
	std::vector<glm::vec2> texture_coords = std::vector<glm::vec2>();

	// Go through every shape that makes up the model
	for (int i = 0; i < shapes.size(); i++) {
		//printf("adding positions (i,max_i) = (%d,%d)\n", i, shapes.size());

		// Go through every vertex
		for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
			//printf("adding positions (j,max_j) = (%d,%d)\n", j, shapes[i].mesh.indices.size()); //   / 100.f
			vertices.push_back(glm::vec3(
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3] * 5.f,
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 1] * 5.f,
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 2] * 5.f
			));

			int texture_index = shapes[i].mesh.material_ids[(int)floor((float)j / 3.f)];
			//const char* texture_index_offset = materials[texture_index].diffuse_texname.c_str();

			//GLuint texture_index_offset = texture_map.find(materials[texture_index].diffuse_texname.c_str())->second;

			//colours.push_back();

			// Assign texture to triangle
			//printf("adding text ids\n");
			texture_ids.push_back(texture_index);

			// If this is a textured vertex
			if (shapes[i].mesh.texcoords.size() != 0) {


				// Assign point of texture to sample
				//printf("adding tex_coords (%d of %d)\n", shapes[i].mesh.indices[j] * 2 + 1, shapes[i].mesh.texcoords.size());
				texture_coords.push_back(glm::vec2(
					shapes[i].mesh.texcoords[shapes[i].mesh.indices[j] * 2],
					shapes[i].mesh.texcoords[shapes[i].mesh.indices[j] * 2 + 1]
				));

				//printf("material[%d].diffuse_texname = %s\n", i, materials[i].diffuse_texname);
			}
			else {
				// Assign texture to triangle
				//printf("vertex is coloured, no texture\n");
				texture_coords.push_back(glm::vec2(0.f, 0.f));
			}
		}
	}

	if (spaceship) {
		SpaceshipGroup* spaceship_complex_model = new SpaceshipGroup(p, s, a, materialMap, vertices, normals, texture_ids, texture_coords);
		return spaceship_complex_model;
	}
	else {
		EntityGroup* spaceship_complex_model = new EntityGroup(p, s, a, materialMap, vertices, normals, texture_ids, texture_coords);
		return spaceship_complex_model;
	}
	
}

void SetupScenes() {
	scenes[0] = new Scene();


	scenes[1] = new Scene();//glm::vec3(0.f, 0.f, -15.f));
	scenes[1]->SetBackground(glm::vec3(0.1f, 0.1f, 0.1f));

	// Load planets
	Entity* sun = new Entity(Entity::Sphere, glm::vec3(-2500.f, 50.f, -5000.f), glm::vec3(200.f, 200.f, 200.f), glm::vec3(0.f, 0.f, 0.f), 100, sun_texture, { 1.f, 1.f, 1.f });
	sun->SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	scenes[1]->AddEntity(sun);

	Entity* e2 = new Entity(Entity::Sphere, glm::vec3(-150.f, 50.f, -250.f), glm::vec3(200.f, 200.f, 200.f), glm::vec3(0.f, 0.f, 0.f), 100, saturn_texture, { 1.f, 1.f, 1.f });
	e2->SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	scenes[1]->AddEntity(e2);

	Entity* e3 = new Entity(Entity::Sphere, glm::vec3(175.f, 50.f, -250.f), glm::vec3(10.f, 10.f, 10.f), glm::vec3(0.f, 0.f, 0.f), 100, titan_texture, { 1.f, 1.f, 1.f });
	e3->SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	scenes[1]->AddEntity(e3);

	// Load models
	EntityGroup* complex_spaceship_model = loadModelAt("models/MyModel3/", "models/MyModel3/station.obj", 
		glm::vec3(100.f, 40.f, -125.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 1.f, 1.f), false);
	complex_spaceship_model->en.at(0).SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	scenes[1]->AddGroup(complex_spaceship_model);

	// Demo spaceship
	//Entity* e4 = new Entity(Entity::Sphere, glm::vec3(50.f, 0.f, -125.f), glm::vec3(5.f, 5.f, 5.f), glm::vec3(0.f, 0.f, 0.f), 100, default_texture, { 0.2f, 0.2f, 0.9f });
	//e4->SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	////SpaceshipGroup* spaceship_model = new SpaceshipGroup(*e4);
	//scenes[1]->AddEntity(*e4);

	// Demo enemy
	//Entity* e5 = new Entity(Entity::Sphere, glm::vec3(-10.f, 15.f, -2.f), glm::vec3(5.f, 5.f, 5.f), glm::vec3(0.f, 0.f, 0.f), 100, default_texture, { 0.9f, 0.2f, 0.2f });
	//e5->SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	//SpaceshipGroup* enemy_spaceship_model = new SpaceshipGroup(*e5);
	SpaceshipGroup* enemy_spaceship_model = (SpaceshipGroup*)loadModelAt("models/MyModel2/", "models/MyModel2/enemy.obj",//"models/MyModel1/", "models/MyModel1/model.obj",//"models/MyModel2/", "models/MyModel2/enemy.obj",
		glm::vec3(-15.f, 15.f, -2.f), glm::vec3(1.f, 1.f, 1.f), glm::vec3(-3.141/2, -1.f, -1.f), true);
	enemy_spaceship_model->en.at(0).SetVelocity(glm::vec3(0., 0., 0.), Entity::Static);
	
	std::vector<Weapon> enemy_weapons;
	Weapon w1 = Weapon(glm::vec3(-1.0f, 15.5f, 0.f), Weapon::Cannon, 0.1f);
	enemy_weapons.push_back(w1);

	enemy_spaceship_model->AddWeaponSystem(enemy_weapons, &complex_spaceship_model->en.at(0), scenes[1]);
	
	scenes[1]->groups.push_back(enemy_spaceship_model);

	Entity* back = new Entity(Entity::Background, glm::vec3(0.f, 0.f, 0.f), glm::vec3(10000.f, 10000.f, 10000.f), glm::vec3(0.f, 5.5f, 0.f), 100, space_texture, { 1.f, 1.f, 1.f });
	scenes[1]->AddEntity(back);



	// Add camera movement
	Scene::CameraAction a1;
	float* mov = new float[3]{ 400.f, 0.002f, 0.f }; float* startPoint = new float[2]{ 1.5f, 0.f };
	a1.type = Scene::Orbit; a1.target = e2; a1.direction = glm::vec3(0.f, 0.f, 0.f); a1.amount = mov; a1.startPoint = startPoint; a1.currentTicks = 0; a1.totalTicks = 1000;
	scenes[1]->tour.push_back(a1);

	Scene::CameraAction a3;
	float* mov3 = new float[3]{ 0.f, -0.03f, 0.04f }; float* startPoint3 = new float[3]{ -10.f, -15.f, -30.f };
	a3.type = Scene::Lerp; a3.target = nullptr; a3.direction = glm::vec3(0.0f, -0.5f, 0.f); a3.amount = mov3; a3.startPoint = startPoint3; a3.currentTicks = 0; a3.totalTicks = 1200;
	scenes[1]->tour.push_back(a3);

	Scene::CameraAction a2;
	float* mov2 = new float[3]{ 15.f, -0.02f, 0.f }; float* startPoint2 = new float[2]{ -3.14f, 0.5f };
	a2.type = Scene::Orbit; a2.target = &complex_spaceship_model->en.at(0); a2.direction = glm::vec3(0.2f, 4.14f, 0.f); a2.amount = mov2; a2.startPoint = startPoint2; a2.currentTicks = 0; a2.totalTicks = 800;
	scenes[1]->tour.push_back(a2);
}

GLuint InitialiseShader(GLchar* vertex_source, GLchar* fragment_source) {

	/* Assign our handles a "name" to new shader objects */
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	/* Associate the source code buffers with each handle */
	glShaderSource(vshader, 1, (const GLchar**)&vertex_source, 0);
	glShaderSource(fshader, 1, (const GLchar**)&fragment_source, 0);
	/* Compile our shader objects */
	glCompileShader(vshader);
	glCompileShader(fshader);


	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check Vertex Shader
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(vshader, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		//printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Check Fragment Shader
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(fshader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		//printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	/* Assign our program handle a "name" */
	GLuint shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vshader);  /* Attach our shaders to our program */
	glAttachShader(shaderprogram, fshader);
	glBindAttribLocation(shaderprogram, 0, "in_Position");   /* Bind attribute 0 (coordinates) to in_Position and attribute 1 (colors) to in_Color */
	glBindAttribLocation(shaderprogram, 1, "in_Color");
	glLinkProgram(shaderprogram);  /* Link our program, and set it as being actively used */
								   //CheckShader(shaderprogram, "Basic Shader");


	glGetProgramiv(shaderprogram, GL_LINK_STATUS, &Result);
	glGetProgramiv(shaderprogram, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(shaderprogram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		//printf("%s\n", &ProgramErrorMessage[0]);
	}


	return shaderprogram;
}

void SetupShaders(void) {
	s1program = InitialiseShader(filetobuf("./no_light.vert"), filetobuf("./no_light.frag"));
	s2program = InitialiseShader(filetobuf("./light.vert"), filetobuf("./light.frag"));
	s3program = InitialiseShader(filetobuf("./tex_shader.vert"), filetobuf("./tex_shader.frag"));
}

void Render() {
	scenes[1]->Render(s3program, vao);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if ((key == GLFW_KEY_P) && (action == GLFW_PRESS || action == 2) && !scenes[1]->onTour)
		scenes[1]->cameraPosition = glm::vec3(0.f, -20.f, -20.0f);

	if ((key == GLFW_KEY_DOWN) && (action == GLFW_PRESS || action == 2))
		if (speed > 0.01f) speed -= 0.01f;
	if ((key == GLFW_KEY_UP) && (action == GLFW_PRESS || action == 2))
		speed += 0.01f;


	if ((key == GLFW_KEY_LEFT) && (action == GLFW_PRESS || action == 2))
		scenes[1]->cameraAngle.y -= speed;

	if ((key == GLFW_KEY_RIGHT) && (action == GLFW_PRESS || action == 2))
		scenes[1]->cameraAngle.y += speed;

	if ((key == GLFW_KEY_PAGE_UP) && (action == GLFW_PRESS || action == 2))
		scenes[1]->cameraAngle.x -= speed;

	if ((key == GLFW_KEY_PAGE_DOWN) && (action == GLFW_PRESS || action == 2))
		scenes[1]->cameraAngle.x += speed;

	if ((key == GLFW_KEY_A) && (action == GLFW_PRESS || action == 2))
		scenes[1]->cameraPosition.x += speed;

	if ((key == GLFW_KEY_D) && (action == GLFW_PRESS || action == 2))
		scenes[1]->cameraPosition.x -= speed;

	if ((key == GLFW_KEY_T) && (action == GLFW_PRESS || action == 2))
		scenes[1]->onTour = !scenes[1]->onTour;
}

void init() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

int main() {
	int k = 0;

	init();


	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "COMP3214 Final Coursework", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);


	if (nullptr == window) {
		std::cout << "Failed to create GLFW window" << std::endl;

		getc(stdin);
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialise GLEW" << std::endl;

		getc(stdin);
		return EXIT_FAILURE;
	}

	glfwSetKeyCallback(window, key_callback);
	fprintf(stderr, "GL INFO %s\n", glGetString(GL_VERSION));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	//texture_map = std::map<std::string, GLuint>();
	default_texture = loadTexture("textures/default.jpg");
	sun_texture = loadTexture("textures/4k_sun.jpg");
	saturn_texture = loadTexture("textures/4k_saturn.jpg");
	mars_texture = loadTexture("textures/mars.jpg");
	titan_texture = loadTexture("textures/4k_titan.jpg");
	space_texture = loadTexture("textures/space_2.jpg");
	//texture_map.insert(std::make_pair("default", default_texture));
	//texture_map.insert(std::make_pair("saturn", saturn_texture));
	//texture_map.insert(std::make_pair("titan", titan_texture));
	//texture_map.insert(std::make_pair("space", space_texture));

	SetupScenes();
	SetupShaders();

	current_scene = 1;
	scenes[1]->FreeGeometry();
	scenes[1]->SetupGeometry();

	glUseProgram(s3program);


	glViewport(0, 0, screenWidth, screenHeight);
	//glFrustum(left,right,bottom,top,near,far).
	//glFrustum(0., 100., 100., 100., 0., 1000.);

	//scenes[2]->Rotate(glm::vec3(0.01f, 0.f, 0.f));

	while (!glfwWindowShouldClose(window)) {  // Main loop

		// Update scene
		scenes[current_scene]->Update(current_scene);

		// Render scene
		Render();        // OpenGL rendering goes here...
		glfwSwapBuffers(window);        // Swap front and back rendering buffers
		glfwPollEvents();         // Poll for events.

	}

	glfwTerminate();  // Close window and terminate GLFW

	return EXIT_SUCCESS;
}