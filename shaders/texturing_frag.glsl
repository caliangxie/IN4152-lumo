#version 450 core

// Global variables for lighting calculations.
layout(location = 2) uniform sampler2D interpNormals;
layout(location = 3) uniform sampler2D textureMap;

layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

void main() {
	// Map using correct coordinates later
	vec2 normalCoords = gl_FragCoord.xy / textureSize(interpNormals, 0).x;
	normalCoords.y = 1.0 - normalCoords.y;

	vec2 texCoords = texture(interpNormals, normalCoords).xy;
	outColor = texture(textureMap, texCoords);
}