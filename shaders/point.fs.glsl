#version 430 core

in vec3 vPos;

float radius = 80.0;
vec3 cold = vec3(1.0, 0.0, 0.0);
vec3 hot = vec3(0.0, 0.0, 1.0);

void main(void) {
	// Color is based on distance from origin
	gl_FragColor = vec4(mix(cold, hot, length(vPos) / radius), 1.0);
}