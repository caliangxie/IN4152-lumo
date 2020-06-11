#version 450 core

// Global variables for lighting calculations.
//layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D tex_normals;

 
// Interpolated output data from vertex shader.

layout(origin_upper_left) in vec4 gl_FragCoord; // window relative fragment coordinates

// Output for on-screen color
layout(location = 0) out vec4 outColor;

// check horizontal outline values  for current position and return them
// Left grad first , right grad sec
// plus coordinates in vec4 (x,y, R, G), if not found put 0.0 on rg components


void get_hor_normals(inout vec4 normals[2], sampler2D tex, vec2 coord)
{
	float step_size = 1.0 / textureSize(tex_normals, 0).x;
	int text_size =  textureSize(tex_normals, 0).x;

	// initialization
	normals[0] = vec4(coord, 0.0, 0.0);
	normals[1] = vec4(coord, 0.0, 0.0);

	float y_coord = 0.0;
	// find normal to the left 
	for (y_coord; y_coord <  coord.y ; y_coord += step_size){
		// update location
		normals[0].y = y_coord;

		// get texture value
		normals[0].zw = (texture(tex_normals, normals[0].xy)).xy ;

		// Check if there is a gradient at current location
		bool bool_cond = all( equal( normals[0].zw , vec2(0.0, 0.0)) );
		// in case there is break this loop
		if( !bool_cond){
			break;
		}
	}



	// find normal to the right
    y_coord = coord.y + step_size;
	for (y_coord; y_coord <  1.0 ; y_coord += step_size){
		// update location
		normals[1].y = y_coord;

		// get texture value
		normals[1].zw = (texture(tex_normals, normals[1].xy)).xy ;


		// Check if there is a gradient at current location
		bool bool_cond = all( equal( normals[1].zw , vec2(0.0, 0.0)) );
		// in case there is break this loop
		if( !bool_cond){
			break;
		}
	}

}

void main() {

	// Subtract from 1 because otherwise coordinates are flipped across both axes
	vec2 tex_coords = gl_FragCoord.xy / textureSize(tex_normals, 0).x;
	tex_coords.y = 1.0 - tex_coords.y;


		
	vec4 normals[2];
	
	get_hor_normals( normals, tex_normals , tex_coords);



	// retrieve current element RG colour 
	vec2 curr_col = (texture(tex_normals, tex_coords)).xy;

	// check if components are black
	bool bool_cond = all( equal( curr_col , vec2(0.0, 0.0)) );


	if( bool_cond){
		curr_col = vec2(1.0,1.0);
	}

	// debug output 
	outColor = vec4( curr_col, 0.0, 1.0);

	
}