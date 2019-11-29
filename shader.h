// Code based off Angel's InitShader.cpp

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>

#include <GL/glew.h>

struct Shader
{
	std::string file;
	GLenum type;
	Shader(std::string f, GLenum t)
		: file(f), type(t)
	{}
};

GLuint loadShader(std::vector<Shader> &shaders);

#endif