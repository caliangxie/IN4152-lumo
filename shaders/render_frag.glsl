#version 450 core

// Global variables for lighting calculations.
layout(location = 2) uniform sampler2D tex;

layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

void main() {
	vec2 texCoords = gl_FragCoord.xy / textureSize(tex, 0).xy;
	texCoords.y = 1.0 - texCoords.y;

	outColor = texture(tex, texCoords);
}