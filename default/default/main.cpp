#include <stdio.h>



//////////////////////////////////////////////////////////
// setup created by Alex Everett						//
//														//
// this solution will create a visual studio project	//
// with all the includes pointed correctly and relative	//
// to the project										//
//														//
// this includes:	GLFW, GLEW,							//
//					GLM,								//
//					Bullet,								//
//					tiny_obj_loader,					//
//					stb_image.							//
//														//
// this will compile bullet into the program so to get	//
// around linking a libs								//
//														//
// only tested for x64									//
///////////////////////////////////////////////////////////



// OpenGL (GLFW/GLEW)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM (basics)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Bullet Physics (basics)
#include <btBulletDynamicsCommon.h>

// tiny_obj_loader
#define TINYOBJLOADER_IMPLEMENTATION // only define in one file
#include <tiny_obj_loader.h>

// stb_image
#define STB_IMAGE_IMPLEMENTATION // only define in one file
#include <stb_image.h>



int main()
{
	printf("If you see this, the setup probably works. :)");
	return 0;
}
