#version 430 core

layout (location = 1) uniform sampler2D tex;

in vec2 vTexCoord;

void main(void) {
	gl_FragColor = texture2D(tex, vTexCoord);
}