// Code based off Angel's InitShader.cpp

#include "shader.h"

#include <iostream>
#include <fstream>

using namespace std;

// Load given path into buffer
static char* readFile(const char* file)
{
	// Get file pointer
	FILE* fp = fopen(file, "r");
	if (fp == NULL)
		return NULL;

	// Get length of file
	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	// Copy file into buffer
	char* buf = new char[size + 1];
	if (size != fread(buf, 1, size, fp)) {
		delete [] buf;
		return NULL;
	}

	// Null terminate string and close
	buf[size] = '\0';
	fclose(fp);

	return buf;
}

static GLuint compileShader(const string file, GLenum type)
{
	GLuint shader;

	// Load shader from file
	char *source = readFile(file.c_str());
	if (source == NULL) {
		std::cerr << "Failed to read " << file << std::endl;
		return 0;
	}

	// Give shader to to GL to compile
	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar **) &source, NULL);
	delete [] source;
	glCompileShader(shader);

	// Check that program compiled correctly
	GLint compileOk;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileOk);
	if (!compileOk) {
		cerr <<  "Failed to compile: " << endl;

		GLint logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		char* logMsg = new char[logSize];
		glGetShaderInfoLog(shader, logSize, NULL, logMsg);
		cerr << logMsg << endl;
		delete [] logMsg;

		return 0;
	}

	return shader;
}

GLuint loadShader(vector<Shader> &shaders)
{
	// Get GL reference to program
	GLuint program = glCreateProgram();

	// Compile each shader
	vector<GLuint> ids(shaders.size());
	for (int i = 0; i < shaders.size(); i++) {
		ids[i] = compileShader(shaders[i].file, shaders[i].type);

		if (ids[i] == 0)
			return 0;

		glAttachShader(program, ids[i]);
	}

	// Link compiled code togeather
	glLinkProgram(program);

	// Check that linking was successful
	GLint link_ok;
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		cerr << "Failed to link shader program" << endl;

		GLint logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

		vector<char> infoLog(logSize);
		glGetProgramInfoLog(program, logSize, &logSize, &infoLog.front());
		cerr << "Length: " << logSize << endl;
		cerr << &infoLog.front() << endl;

		return 0;
	}

	return program;
}