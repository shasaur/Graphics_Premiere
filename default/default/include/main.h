#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <iostream>
#include <stdio.h>

#pragma comment(linker, "/STACK:2000000000")
#pragma comment(linker, "/HEAP:2000000000")

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


#include "VertexBufferObject.h"

// OpenGL (GLFW/GLEW)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM (basics)
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
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

// properties
#include "World.h"

// other
#include "glslprogram.h"
#include "utils.h"

// My base classes
#include "Vertex.h"
#include "entity.h"
#include "scene.h"
#include "entitygroup.h"
//#include "stb_image.h"


#endif // MAIN_H_INCLUDED