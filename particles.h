#ifndef PARTICLES_H
#define PARTICLES_H

#include "external-files/glew/glew.h"

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "external-files/glm/glm.hpp"
#include "external-files/tiny_obj_loader.h"

#include "camera.h"

class Particles
{
	public:
		// Load model, set maximum particles
		Particles(const std::string &obj,
				  const std::string &mtl,
				  int blockSize,
				  int maxParticles);
		~Particles();
		
		// Add num particles with provided function
		void addParticles(int num, glm::vec4 (*posW)(), glm::vec4 (*velO)());

		// Draw particles
		void draw(Camera &camera, bool points);

		// Move simulation ahead a single time step
		void step(float timeStep);

	private:
		// Holds basic models data
		tinyobj::shape_t m;

		// Vertex array buffers for easy rebinding
		// Two are used to allow simultaneous rendering & computing
		GLuint vao[2];

		// Vertex/texcoord and index buffer
		GLuint vbo, ibo;

		// References to different glsl programs
		GLuint computeProgram, renderProgram, pointProgram;

		// Render program attribute locations
		enum renderAttributes
		{
			vert,		// 0
			texCoord,	// 1
			pos			// 2
		};

		// Render program uniform locations
		enum renderUniforms
		{
			mvp,		// 0

			ambient,	// 1
			diffuse,	// 2
			specular,	// 3
			shininess,	// 4

			lightColor, // 5
			lightDir,	// 6
			halfVec,	// 7

			tex,		// 8

			cameraPos,	// 9
			viewDir,	// 10
			frusNorms	// 11-14
		};

		// Holds reference to loaded texture
		GLint texture;

		// Instance data (positions/velocity/weights/other) in shader storage buffer object used for simulation
		// Data is interleaved, first position/weight, then velocity/other
		// Format: 2 vec4's, (PPPWVVVO)(PPPWVVVO)...
		// Note: Other isn't used, but is useful since it keeps the memory alligned
		GLuint pvw[2];

		// Know whether to draw or compute pvw[0]
		// Opposite applies to pvw[1]
		int pvwIndex;

		// Number of current particles
		const int maxP;
		int numP;

		// Compute shader work group size
		const unsigned int WORK_GROUP_SIZE;
};

#endif