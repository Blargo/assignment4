#version 430 core

layout (triangles, fractional_even_spacing, ccw) in;

layout (location = 0) uniform mat4 mvp;

in vec2 tcTexCoord[];
patch in vec4 tcPos;

out vec3 teNormal;
out vec2 teTexCoord;
out vec4 tePos;

void main(void) {
	// Calculate vextex based on calculated barycentric coordinates
	vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;
	vec3 p = normalize(p0 + p1 + p2);

	// Calculate texture coordinate
	vec2 t0 = gl_TessCoord.x * tcTexCoord[0];
	vec2 t1 = gl_TessCoord.y * tcTexCoord[1];
	vec2 t2 = gl_TessCoord.z * tcTexCoord[2];
	teTexCoord = t0 + t1 + t2;

	// Note: since this is a unit sphere, the vextex is also the normal
	teNormal = p;
	tePos = tcPos;

	// Scale and translate verts then pass on
	gl_Position = mvp * vec4(p * (tePos.w * 0.15) + tePos.xyz, 1);
}