#include "main.h"

int renderMode = 1;
bool pause = false;

//void Check(const char *where) { // Function to check OpenGL error status
//	const char * what;
//	int err = glGetError();   //0 means no error
//	if (!err)
//		return;
//	if (err == GL_INVALID_ENUM)
//		what = "GL_INVALID_ENUM";
//	else if (err == GL_INVALID_VALUE)
//		what = "GL_INVALID_VALUE";
//	else if (err == GL_INVALID_OPERATION)
//		what = "GL_INVALID_OPERATION";
//	else if (err == GL_INVALID_FRAMEBUFFER_OPERATION)
//		what = "GL_INVALID_FRAMEBUFFER_OPERATION";
//	else if (err == GL_OUT_OF_MEMORY)
//		what = "GL_OUT_OF_MEMORY";
//	else
//		what = "Unknown Error";
//	fprintf(stderr, "Error (%d) %s  at %s\n", err, what, where);
//}
//void CheckShader(int sp, const char *x) {
//	int length;
//	char text[1001];
//	glGetProgramInfoLog(sp, 1000, &length, text);   // Check for errors
//	if (length > 0) {
//		fprintf(stderr, "Validate Shader Program\nMessage from:%s\n%s\n", x, text);
//	}
//}
//char* filetobuf(char *file) { /* A simple function that will read a file into an allocated char pointer buffer */
//	FILE *fptr;
//	long length;
//	char *buf;
//	fprintf(stderr, "Loading %s\n", file);
//#pragma warning (disable : 4996)
//	fptr = fopen(file, "rb");   /* Open file for reading */
//	if (!fptr) {   /* Return NULL on failure */
//		fprintf(stderr, "failed to open %s\n", file);
//		return NULL;
//	}
//	fseek(fptr, 0, SEEK_END);   /* Seek to the end of the file */
//	length = ftell(fptr);   /* Find out how many bytes into the file we are */
//	buf = (char*)malloc(length + 1);   /* Allocate a buffer for the entire length of the file and a null terminator */
//	fseek(fptr, 0, SEEK_SET);   /* Go back to the beginning of the file */
//	fread(buf, length, 1, fptr);   /* Read the contents of the file in to the buffer */
//	fclose(fptr);   /* Close the file */
//	buf[length] = 0;   /* Null terminator */
//	return buf;   /* Return the buffer */
//}

/* This is a handle to the shader program */
GLuint s1program, s2program, s3program;
GLuint vao, vbo[1]; /* Create handles for our Vertex Array Object and One Vertex Buffer Object */

std::vector<Vertex> v;

Scene* scenes[4];
int current_scene = 0;

GLuint mars_texture;

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

float rnd(float limit, float offset) {
	return ((float)(rand() % 100) / (100.f / limit)) - offset;
}

void AddPhysicalObject(Scene &scene, Entity::Shape shape, float size, glm::vec3 pos, glm::dvec3 velocity);

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

void SetupScenes() {
	scenes[0] = new Scene();
	scenes[1] = new Scene(glm::vec3(0.f, 0.f, -15.f));

	//Entity e1(Entity::Cube, glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.5f, 1.5f, 1.5f), glm::vec3(0.f, 0.f, 0.f), 40, true);
	//scenes[0]->AddEntity(e1);

	//AddPhysicalObject(*scenes[1], Entity::Cube, 0.5f);
	Entity e2(Entity::Cube, glm::vec3(rnd(20, 10), rnd(20, 10), rnd(20, 10)), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), 40, false);
	e2.SetVelocity(btVector3(rnd(6, 3), rnd(6, 3), rnd(6, 3)));
	scenes[1]->AddEntity(e2);

	Entity e3(Entity::Sphere, glm::vec3(rnd(20, 10), rnd(20, 10), rnd(20, 10)), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), 40, false);
	e3.SetVelocity(btVector3(rnd(6, 3), rnd(6, 3), rnd(6, 3)));
	scenes[1]->AddEntity(e3);

	Entity e4(Entity::Sphere, glm::vec3(rnd(20, 10), rnd(20, 10), rnd(20, 10)), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.f, 0.f, 0.f), 40, false);
	e4.SetVelocity(btVector3(rnd(6, 3), rnd(6, 3), rnd(6, 3)));
	scenes[1]->AddEntity(e4);

	//for (int i = 0; i < 3; i++) {
	//	AddPhysicalObject(*scenes[1], Entity::Sphere, 0.5f);
	//}

	// AddPhysicalObject(*scenes[1], Entity::Cube, 0.5f, glm::vec3(0,-6,-16), glm::dvec3(3,2,0));
	// AddPhysicalObject(*scenes[1], Entity::Cube, 0.5f, glm::vec3(0,-3,9), glm::dvec3(3,2,0));
	// AddPhysicalObject(*scenes[1], Entity::Sphere, 0.5f, glm::vec3(0,-3,-30), glm::dvec3(-2,4,0));
	// AddPhysicalObject(*scenes[1], Entity::Sphere, 0.5f, glm::vec3(0,-3,9), glm::dvec3(2,-2,2));
	// AddPhysicalObject(*scenes[1], Entity::Sphere, 0.5f, glm::vec3(0,3,21), glm::dvec3(0,0,0));

	scenes[1]->SetBackground(glm::vec3(0.1f, 0.1f, 0.1f));
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
	glUseProgram(s1program);
}

void Render() {
	scenes[1]->Render(s2program, vao);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void init() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}

const GLint WIDTH = 600, HEIGHT = 600;

int main() {
	int k = 0;

	init();


	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "COMP3214 Coursework Physics", nullptr, nullptr);

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
	//glDepthMask(GL_FALSE);

	mars_texture = loadTexture("textures/mars.bmp");

	SetupScenes();
	SetupShaders();

	current_scene = 1;
	scenes[1]->FreeGeometry();
	scenes[1]->SetupGeometry();

	glUseProgram(s2program);


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