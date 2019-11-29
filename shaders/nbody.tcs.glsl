#version 430 core

layout (vertices = 3) out;

layout (location = 9) uniform vec3 cameraPos;
layout (location = 10) uniform vec3 viewDir;
layout (location = 11) uniform vec3 frusNorms[4];

in vec2 vTexCoord[];
in vec4 vPos[];

out vec2 tcTexCoord[];
patch out vec4 tcPos;

void main(void) {
	tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];
	tcPos = vPos[gl_InvocationID];

	// Only set LOD once per patch
	if (gl_InvocationID == 0) {
		float tessLevel = 0;

		// Cull patches outside viewing frustrum
		float radius = -tcPos.w * 0.15;
		if (dot(viewDir, tcPos.xyz - cameraPos + vec3(0, 0, 0.1)) >= radius &&
			dot(-viewDir, tcPos.xyz - cameraPos + vec3(0, 0, 4000)) >= radius &&
			dot(frusNorms[0], tcPos.xyz - cameraPos) >= radius &&
			dot(frusNorms[1], tcPos.xyz - cameraPos) >= radius &&
			dot(frusNorms[2], tcPos.xyz - cameraPos) >= radius &&
			dot(frusNorms[3], tcPos.xyz - cameraPos) >= radius)
		{
			// Get distance between particle and camera
			float dist = distance(vPos[gl_InvocationID].xyz, cameraPos);
			tessLevel = 16.6038045 * pow(dist, -0.5908648);
		}

		gl_TessLevelInner[0] = tessLevel;
		gl_TessLevelOuter[0] = tessLevel;
		gl_TessLevelOuter[1] = tessLevel;
		gl_TessLevelOuter[2] = tessLevel;
	}

	// Pass though data
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}