#version 450 core

// Global variables for lighting calculations.
//layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D approxNormals;

 
// Interpolated output data from vertex shader.

layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

void main() {
	// Subtract from 1 because otherwise coordinates are flipped across both axes
	vec2 texCoords = gl_FragCoord.xy / textureSize(approxNormals, 0).x;
	texCoords.y = 1.0 - texCoords.y;

	// retrieve current element RG colour 
	vec2 curr_col = texture(approxNormals, texCoords).xy;

	// debug output 
	outColor = vec4( curr_col, 0.0, 1.0);

	
}