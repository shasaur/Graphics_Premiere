#ifndef CW1_H
#define CW1_H

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



#include <iostream>
#include <stdio.h>

#include <cstdlib>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#include <vector>


#define GLEW_STATIC
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

// My base classes
#include "Vertex.h"
#include "entity.h"
#include "scene.h"
#include "entitygroup.h"
#include "stb_image.h"

#include <fstream>
#include <sstream>

#define PI 3.14159265359f

// Bullet-related
#include "World.h"
#include <btBulletDynamicsCommon.h> //Set up bullet - globals

#endif /* CW1_H */