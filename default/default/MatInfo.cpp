#include "MatInfo.h"

MatInfo::MatInfo(glm::vec3 c, GLuint t, const char* n, const char* fn)
{
	colour = c;
	textId = t;
	name = n;
	fileName = fn;
}