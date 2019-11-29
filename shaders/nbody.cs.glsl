#version 430 core

layout (local_size_x = 256) in;

struct state
{
	vec4 pos;
	vec4 vel;
};

layout (std430, binding = 0) buffer data_in
{
	state data[];
} read;

layout (std430, binding = 1) buffer data_out
{
	state data[];
} write;

// Time step
layout (location = 0) uniform float t;

// Gravitational constant
const float GC = 10;

// Bounding sphere radius
const float BOUND = 80;

// Load in blocks of data into local memory for much faster access
shared vec4 cache[gl_WorkGroupSize.x];

void main(void)
{
	uint i, j;
	uint index = gl_GlobalInvocationID.x;

	// Load in pos and velocity
	state x = read.data[index];

	// Update position
	x.pos.xyz += t * x.vel.xyz;

	// If particle is outside sphere bound
	if (length(x.pos.xyz) > BOUND) {
		// Rebound back where it came from at reduced speed
		x.vel.xyz *= -0.05;
		x.pos = normalize(x.pos) * BOUND;
	}

	// For each workgroup
	for (i = 0; i < gl_NumWorkGroups.x; i++) {
		// Load a groups positions into local memory
		cache[gl_LocalInvocationID.x] = read.data[i * gl_WorkGroupSize.x + gl_LocalInvocationID.x].pos;

		// Sync cache load with local group
		barrier();

		// For each other position
		for (j = 0; j < gl_WorkGroupSize.x; j++) {
			// Calculate distance between particles
			vec4 pos2 = cache[j];
			vec3 dist = x.pos.xyz - pos2.xyz;

			// Update velocity
			x.vel.xyz -= (t * t * pos2.w * normalize(dist)) / (dot(dist, dist) + GC);
		}

		// Ensure that all invocations have finished processing
		barrier();
	}

	// Write result to write buffer
	write.data[index] = x;
}