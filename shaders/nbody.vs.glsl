#version 430 core

layout (location = 0) in vec4 vert;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 pos;

out vec2 vTexCoord;
out vec4 vPos;

// Just a straight through vertex shader
void main(void) {
	vTexCoord = texCoord;
	vPos = pos;
	gl_Position = vert;
}