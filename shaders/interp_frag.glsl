#version 450 core

// Global variables for lighting calculations.
//layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D approxNormals;

//layout(location = 3) uniform mat4 lightMVP;
//layout(location = 4) uniform vec3 lightPos = vec3(3, 3, 3);
 
// Interpolated output data from vertex shader.
//in vec3 fragPos; // World-space position
//in vec3 fragNormal; // World-space normal
layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

void main() {
	// Subtract from 1 because otherwise coordinates are flipped across both axes
	vec2 texCoords = 1.0 - gl_FragCoord.xy / textureSize(approxNormals, 0).x;

	outColor = texture(approxNormals, texCoords);

	//mat3 grad_kernel =  mat3(0, 1, 0, 1, -4, 1, 0, 1, 0);
	// Convolve kernel with texture
	//vec3 col = convolution( grad_kernel, tex_drawing, tex_coords);
	
	// Output to screen
	//outColor = vec4(col, 1.0);

	//outColor =  texture(tex_drawing, tex_coords);
}