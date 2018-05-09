#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

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
//#define STB_IMAGE_IMPLEMENTATION // only define in one file
//#include <stb_image.h>


////////////////////////////////////////////////////
// TOMS STUFF //

#include "World.h"

const double PI = 3.141592653589793;
const double PIo2 = PI / 2.;
const double PIo4 = PI / 4.;
const double PI2 = PI * 2.;
const float lod = PI / 32.;

#include "glslprogram.h"
//#include "stb_image.h"
#include "utils.h"

///////////////////////////////////////////////////

#endif // MAIN_H_INCLUDED