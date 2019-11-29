#ifndef SKYBOX_H
#define SKYBOX_H

#include "external-files/glew/glew.h"

// Note: This was never implemented due to lack of time

class Skybox
{
	public:
		Skybox(const string obj, const string mtl);
		~Skybox();
		
	private:
		GLuint program;

		GLuint vao;

		GLuint vbo, ibo;
};

#endif

/*
vector<tinyobj::shape_t> model;
GLuint skyProgram;
GLuint skyVao;
GLuint skyVbo, skyIbo;
GLint skyTex;

	// Load in skybox
	tinyobj::LoadObj(model, "data/cube/cube.obj", "data/cube/");

	glGenVertexArrays(1, &skyVao);
	glBindVertexArray(skyVao);

	int posSize = model[0].mesh.positions.size() * sizeof(float);
	int texSize = model[0].mesh.texcoords.size() * sizeof(float);
	glGenBuffers(1, &skyVbo);
	glBindBuffer(GL_ARRAY_BUFFER, skyVbo);
	glBufferData(GL_ARRAY_BUFFER,
				 posSize + texSize,
				 NULL,
				 GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
					0,
					posSize,
					&model[0].mesh.positions.front());
	glBufferSubData(GL_ARRAY_BUFFER,
					posSize,
					texSize,
					&model[0].mesh.texcoords.front());

	glGenBuffers(1, &skyIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 model[0].mesh.indices.size() * sizeof(unsigned int),
				 &model[0].mesh.indices.front(),
				 GL_STATIC_DRAW);

	glVertexAttribPointer(0,		// Attribute index
						  3,    	// Number of elements
						  GL_FLOAT,	// Type of elements
						  GL_FALSE, // Normalise?
						  0,		// Step
						  NULL);   	// Offset
	glVertexAttribPointer(1,		// Attribute index
						  3,    	// Number of elements
						  GL_FLOAT,	// Type of elements
						  GL_FALSE, // Normalise?
						  0,		// Step
						  (void*) posSize); // Offset

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Load texture
	skyTex = SOIL_load_OGL_texture(
		model[0].material.diffuse_texname.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
	if (!skyTex)
		cout << "Failed to load texture: " << SOIL_last_result() << endl;

	// Create program to draw skybox
	vector<Shader> shaders;
	shaders.push_back(Shader("shaders/texture.vs.glsl", GL_VERTEX_SHADER));
	shaders.push_back(Shader("shaders/texture.fs.glsl", GL_FRAGMENT_SHADER));
	skyProgram = loadShader(shaders);
*/

/*
	glUseProgram(skyProgram);
	glBindVertexArray(skyVao);

	glm::mat4 view = camera.getPerp() * camera.getView();
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(view));

	glBindTexture(GL_TEXTURE_2D, 1);

	glDrawElements(GL_TRIANGLES,
				   model[0].mesh.indices.size(),
				   GL_UNSIGNED_INT,
				   NULL);
*/
