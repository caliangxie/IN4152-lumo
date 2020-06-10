#version 450 core

// Global variables for lighting calculations.
layout(location = 2) uniform sampler2D interpNormals;
layout(location = 3) uniform sampler2D textureMap;

layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

void main() {
	// Map using correct coordinates later
	vec2 texCoords = gl_FragCoord.xy / textureSize(textureMap, 0).xy;
	texCoords.y = 1.0 - texCoords.y;

	outColor = texture(textureMap, texCoords);
}