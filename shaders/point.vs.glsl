#version 430 core

layout (location = 0) uniform mat4 mvp;

layout (location = 0) in vec3 pos;

out vec3 vPos;

void main(void) {
	vPos = pos;

	// Point size slightly larger than 1, causes points to twinkle
	gl_PointSize = 1.4;
	gl_Position = mvp * vec4(pos, 1.0);
}