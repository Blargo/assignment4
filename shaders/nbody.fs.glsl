#version 430 core

// Material properties
layout (location = 1) uniform vec3 ambient;
layout (location = 2) uniform vec3 diffuse;
layout (location = 3) uniform vec3 specular;
layout (location = 4) uniform float shininess;

// Light properties
layout (location = 5) uniform vec3 lightColor;
layout (location = 6) uniform vec3 lightDir;
layout (location = 7) uniform vec3 halfVec;

// Texture
layout (location = 8) uniform sampler2D tex;

// Inputs from tessellation evaluation shader
in vec3 teNormal;
in vec2 teTexCoord;
in vec4 tePos;

float strength = 0.7;

// Radius of bounding sphere
float radius = 80;

// Color at origin and bounding sphere respectively
vec3 hot = vec3(1.0, 0.0, 0.0);
vec3 cold = vec3(0.0, 0.0, 1.0);

void main(void) {
	// Calculate intensities
	float diffIntense = max(0.0, dot(teNormal, lightDir));
	float specIntense = diffIntense ? pow(max(0.0, dot(teNormal, halfVec)), shininess) : 0.0;

	// Calculate overall colors
	vec3 heat = mix(hot, cold, length(tePos.xyz) / radius);
	vec4 texture = texture2D(tex, teTexCoord);
	vec3 scattered = diffIntense * diffuse * lightColor * texture.rgb * heat;
	vec3 reflected = strength * specIntense * specular * lightColor;

	// Pass out final color
	gl_FragColor = vec4(min(ambient + scattered + reflected, vec3(1.0)), texture.a);
}