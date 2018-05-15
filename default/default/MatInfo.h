#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

class MatInfo
{
public:
	glm::vec3 colour;
	GLuint textId; // HAS TO BE THE SAME AS TEXTURE_ID, the same thing that you pass in to partition vertices
	const char* name;
	const char* fileName;

	MatInfo(glm::vec3 colour, GLuint textId, const char* name, const char* fileName);
};

