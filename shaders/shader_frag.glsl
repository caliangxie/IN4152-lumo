#version 450 core

// Global variables for lighting calculations.
//layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D tex_drawing;

//layout(location = 3) uniform mat4 lightMVP;
//layout(location = 4) uniform vec3 lightPos = vec3(3, 3, 3);
  
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader.
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal
layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates



void make_kernel(inout vec4 n[9], sampler2D tex, vec2 coord)
{
	float w = 1.0 / textureSize(tex_drawing, 0).x;
	float h = 1.0 / textureSize(tex_drawing, 0).y;

	n[0] = texture(tex, coord + vec2( -w, -h));
	n[1] = texture(tex, coord + vec2(0.0, -h));
	n[2] = texture(tex, coord + vec2(  w, -h));
	n[3] = texture(tex, coord + vec2( -w, 0.0));
	n[4] = texture(tex, coord);
	n[5] = texture(tex, coord + vec2(  w, 0.0));
	n[6] = texture(tex, coord + vec2( -w, h));
	n[7] = texture(tex, coord + vec2(0.0, h));
	n[8] = texture(tex, coord + vec2(  w, h));
}


void main()
{
   // current position
    vec2 tex_coords = gl_FragCoord.xy  / (textureSize(tex_drawing, 0).x) ;


	vec4 n[9];
	// construct 3x3 kernel matrix around texture coordinates 
	make_kernel( n, tex_drawing , tex_coords );

	// compute horzontal and vertical gradient components using sobel's operator
	vec4 sobel_edge_h = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
  	vec4 sobel_edge_v = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
	
	// compute total gradient magnitude
	//vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));
	
	// compute component-wise gradient
	vec3 out_vec =  normalize (vec3(length(sobel_edge_v), length(sobel_edge_h),0.0) );
	
	// render normalized gradient vector as a RG colourspace 
	outColor = vec4( out_vec, 1.0 );

	// output basic underlying texture 
	// outColor =  texture(tex_drawing, tex_coords);
}
