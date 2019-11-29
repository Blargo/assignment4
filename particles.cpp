#include "particles.h"

#include <iostream>
#include <cstring>
#include <vector>

#include "external-files/SOIL/SOIL.h"
#include "external-files/glm/gtc/type_ptr.hpp"
#include "external-files/glm/gtc/matrix_transform.hpp"

#include "shader.h"

using namespace std;

Particles::Particles(const string &obj,
					 const string &mtl,
					 int blockSize,
					 int maxParticles)
	: numP(0),
	  maxP(maxParticles),
	  pvwIndex(0),
	  WORK_GROUP_SIZE(blockSize)
{
	// Load in the model
	vector<tinyobj::shape_t> model;
	tinyobj::LoadObj(model, obj.c_str(), mtl.c_str());
	m = model[0];

	// Set up vertex/texcoord buffer
	// Note: normals are generated in tesselation shader
	size_t posSize = m.mesh.positions.size() * sizeof(float);
	size_t texSize = m.mesh.texcoords.size() * sizeof(float);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
				 posSize + texSize,
				 NULL,
				 GL_STATIC_DRAW);

	// Fill geometry buffer
	glBufferSubData(GL_ARRAY_BUFFER,
					0,
					posSize,
					&m.mesh.positions.front());
	glBufferSubData(GL_ARRAY_BUFFER,
					posSize,
					texSize,
					&m.mesh.texcoords.front());

	// Set up indices buffer
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 m.mesh.indices.size() * sizeof(unsigned int),
				 &m.mesh.indices.front(),
				 GL_STATIC_DRAW);

	// Create position/velocity/weight buffers
	glGenBuffers(2, pvw);
	for (int i = 0; i < 2; i++) {
		glBindBuffer(GL_ARRAY_BUFFER, pvw[i]);
		glBufferData(GL_ARRAY_BUFFER,
					 maxP * 8 * sizeof(GLfloat), // Each particle has 2 vec4 of floats
					 NULL,
					 GL_DYNAMIC_COPY); // Since buffer is read/write and used frequently
	}

	// For each vao, set state
	glGenVertexArrays(2, vao);
	for (int i = 0; i < 2; i++) {
		// Bind vao
		glBindVertexArray(vao[i]);

		// Set geometry attributes
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(vert,		// Attribute index
							  3,    	// Number of elements
							  GL_FLOAT,	// Type of elements
							  GL_FALSE, // Normalise?
							  0,		// Block size
							  NULL);   	// Offset

		glVertexAttribPointer(texCoord,	// Attribute index
							  2,    	// Number of elements
							  GL_FLOAT,	// Type of elements
							  GL_FALSE, // Normalise?
							  0,		// Block size
							  (void*) posSize); // Offset

		// Instance pos attribute
		glBindBuffer(GL_ARRAY_BUFFER, pvw[i]);
		glVertexAttribPointer(pos,		// Attribute index
							  4,		// Number of elements
							  GL_FLOAT,	// Type of elements
							  GL_FALSE, // Normalise?
							  8 * sizeof(GLfloat), // Block size
							  NULL);	// Offset

		// Set pos to change every instance
		glVertexAttribDivisor(pos, 1);

		// Set index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		// Enable necessary attributes
		glEnableVertexAttribArray(vert);
		glEnableVertexAttribArray(texCoord);
		glEnableVertexAttribArray(pos);
	}

	// Load texture
	texture = SOIL_load_OGL_texture(
		m.material.diffuse_texname.c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
	if (!texture)
		cout << "Failed to load texture: " << SOIL_last_result() << endl;

	// Compile shaders
	// Render shader
	vector<Shader> shaders;
	shaders.push_back(Shader("shaders/nbody.vs.glsl", GL_VERTEX_SHADER));
	shaders.push_back(Shader("shaders/nbody.tcs.glsl", GL_TESS_CONTROL_SHADER));
	shaders.push_back(Shader("shaders/nbody.tes.glsl", GL_TESS_EVALUATION_SHADER));
	shaders.push_back(Shader("shaders/nbody.fs.glsl", GL_FRAGMENT_SHADER));
	renderProgram = loadShader(shaders);

	// Compute shader
	shaders.clear();
	shaders.push_back(Shader("shaders/nbody.cs.glsl", GL_COMPUTE_SHADER));
	computeProgram = loadShader(shaders);

	shaders.clear();
	shaders.push_back(Shader("shaders/point.vs.glsl", GL_VERTEX_SHADER));
	shaders.push_back(Shader("shaders/point.fs.glsl", GL_FRAGMENT_SHADER));
	pointProgram = loadShader(shaders);

	// Set constant uniforms in render shader
	glUseProgram(renderProgram);
	glUniform3f(lightColor, 1.0, 1.0, 1.0);
}

Particles::~Particles()
{
	// Delete vaos and buffers and programs
	glDeleteVertexArrays(2, vao);

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(2, pvw);

	glDeleteProgram(renderProgram);
	glDeleteProgram(computeProgram);
	glDeleteProgram(pointProgram);
}

// Add num particles with provided functions
// Note, adding during concurrent operations may be foolish
void Particles::addParticles(int num, glm::vec4 (*posW)(), glm::vec4 (*velO)())
{
	// If particles can't be added
	if (numP >= maxP)
		return;

	// Make sure that all drawing is flushed
	// and that memory writes have taken place
	glMemoryBarrier(GL_SHADER_STORAGE_BUFFER);

	// Bind and map the buffer
	glBindBuffer(GL_ARRAY_BUFFER, pvw[pvwIndex]);
	float *ptr = (float*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	// Move pointer the end of data
	ptr += numP * 8;

	// While can still add particles
	while (num > 0 && numP < maxP) {
		// Pos/weight first
		memcpy(ptr, glm::value_ptr(posW()), 4 * sizeof(GLfloat));
		ptr += 4;

		// Then velocity/other
		memcpy(ptr, glm::value_ptr(velO()), 4 * sizeof(GLfloat));
		ptr += 4;

		// Increment counter
		num--;
		numP++;
	}

	// No longer need access to buffer
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

// Draw and update particles
void Particles::draw(Camera &camera, bool points)
{
	// If rendering as basic points
	if (points) {
		// Set current program
		glUseProgram(pointProgram);

		// Unbind any vaos, don't want to mess with their state
		glBindVertexArray(0);

		// Set mvp matrix
		glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(camera.getPerp() * camera.getView()));

		// Position attribute
		// Note: point program has only 1 input attribute, 
		// Hence it's index is 0
		glBindBuffer(GL_ARRAY_BUFFER, pvw[pvwIndex]);
		glVertexAttribPointer(0,		// Attribute index
							  3,		// Number of elements
							  GL_FLOAT,	// Type of elements
							  GL_FALSE, // Normalise?
							  8 * sizeof(GLfloat), // Step
							  NULL);	// Offset
		glEnableVertexAttribArray(0);

		// Draw points
		glDrawArrays(GL_POINTS, 0, numP);
	} else {
		// Switch to drawing program
		glUseProgram(renderProgram);

		// Set light uniforms
		glUniform3fv(ambient, 1, m.material.ambient);
		glUniform3fv(diffuse, 1, m.material.diffuse);
		glUniform3fv(specular, 1, m.material.specular);
		glUniform1f(shininess, m.material.shininess);
		
		// Bind texture
		glBindTexture(GL_TEXTURE_2D, texture);

		// Set mvp matrix
		glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(camera.getPerp() * camera.getView()));

		// Set light direction
		glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0, 1.0, 1.0));
		glUniform3fv(lightDir, 1, glm::value_ptr(glm::vec4(lightDirection, 0.0)));

		// Set half vector
		glm::vec3 eye = glm::normalize(camera.getPos());
		glUniform3fv(halfVec, 1, glm::value_ptr(glm::normalize(eye + lightDirection)));

		// Set camera data
		glUniform3fv(cameraPos, 1, glm::value_ptr(camera.getPos()));
		glUniform3fv(viewDir, 1, glm::value_ptr(camera.getViewDir()));

		// Set frustum normals
		// Note: since 4 vec3s are being set, they take uniform locations 11 - 14
		vector<glm::vec3> frus = camera.getFrustumNormals();
		for (int i = 0; i < frus.size(); i++) {
			glUniform3fv(frusNorms + 1, 1, glm::value_ptr(frus[i]));
		}

		// Rebind necessary state
		glBindVertexArray(vao[pvwIndex]);

		// Draw numP instanced models
		glDrawElementsInstanced(GL_PATCHES,
								m.mesh.indices.size(),
								GL_UNSIGNED_INT,
								NULL,
								numP);
	}
}

void Particles::step(float timeStep)
{
	// Reload compute program
	glUseProgram(computeProgram);

	// Set simulation timestep
	// Note: there is only 1 uniform in compute shader
	// Hence it's index is 0
	glUniform1f(0, timeStep);

	// Set compute shader storage
	// Set read buffer
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pvw[pvwIndex]);

	// Set write buffer
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pvw[pvwIndex ^ 1]);

	// Begin updating positions with correct number of work groups
	if (numP % WORK_GROUP_SIZE == 0)
		glDispatchCompute(numP / WORK_GROUP_SIZE, 1, 1);
	else
		glDispatchCompute(numP / WORK_GROUP_SIZE + 1, 1, 1);

	// Swap double instance data buffer
	pvwIndex ^= 1;
}
