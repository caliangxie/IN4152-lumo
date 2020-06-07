#version 450 core

// Global variables for lighting calculations.
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D tex_drawing;

//layout(location = 3) uniform mat4 lightMVP;
//layout(location = 4) uniform vec3 lightPos = vec3(3, 3, 3);
  
// Output for on-screen color
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader.
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal
layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates




void main()
{
   // current position
    vec2 tex_coords = gl_FragCoord.xy  / (textureSize(tex_drawing, 0).x) ;



	vec4 n[9];
	make_kernel( n, tex_drawing , tex_coords );

	vec4 sobel_edge_v = n[2] + (2.0*n[5]) + n[8] - (n[0] + (2.0*n[3]) + n[6]);
  	vec4 sobel_edge_h = n[0] + (2.0*n[1]) + n[2] - (n[6] + (2.0*n[7]) + n[8]);
	vec4 sobel = sqrt((sobel_edge_h * sobel_edge_h) + (sobel_edge_v * sobel_edge_v));
	vec3 out_vec =  vec3(length(sobel_edge_v), length(sobel_edge_h),0.0);
	outColor = vec4( normalize(out_vec), 1.0 );




  //  outColor =  texture(tex_drawing, tex_coords);
}