#version 450 core

// Global variables for lighting calculations.
//layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D approxNormals;

 
// Interpolated output data from vertex shader.

layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// check horizontal outline values  for current position and return them
// Left grad first , right grad sec
// plus coordinates in vec4 (x,y, R, G), if nont found put 0.0 on rg components
void get_hor_normals(inout vec4 normals[2], sampler2D tex, vec2 coord)
{
	float step_size = 1.0 / textureSize(tex_drawing, 0).x;
	



}

void main() {
	// Subtract from 1 because otherwise coordinates are flipped across both axes
	vec2 texCoords = gl_FragCoord.xy / textureSize(approxNormals, 0).x;
	texCoords.y = 1.0 - texCoords.y;

	// retrieve current element RG colour 
	vec2 curr_col = (texture(approxNormals, texCoords)).xy;

	// check if components are black
	bool bool_cond = all( equal( curr_col , vec2(0.0, 0.0)) );


	if( bool_cond){
		curr_col = vec2(1.0,1.0);
	}

	// debug output 
	outColor = vec4( curr_col, 0.0, 1.0);

	
}