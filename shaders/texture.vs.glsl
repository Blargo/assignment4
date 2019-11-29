#version 430 core

layout (location = 0) uniform mat4 mvp;

layout (location = 0) in vec4 vert;
layout (location = 1) in vec2 texCoord;

out vec2 vTexCoord;

void main(void) {
	vTexCoord = texCoord;
	gl_Position = mvp * vert;
}