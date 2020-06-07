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


// Find coordinate of matrix element from index
vec2 kpos(int index)
{
    return vec2[9] (
        vec2(-1, -1), vec2(0, -1), vec2(1, -1),
        vec2(-1, 0), vec2(0, 0), vec2(1, 0), 
        vec2(-1, 1), vec2(0, 1), vec2(1, 1)
    )[index] / textureSize(tex_drawing, 0);
}




// Extract region of dimension 3x3 from sampler centered in uv
// sampler : texture sampler
// uv : current coordinates on sampler
// return : an array of mat3, each index corresponding with a color channel
mat3[3] region3x3(sampler2D sampler, vec2 uv)
{
    // Create each pixels for region
    vec4[9] region;
    
    for (int i = 0; i < 9; i++)
        region[i] = texture(sampler, uv + kpos(i));

    // Create 3x3 region with 3 color channels (red, green, blue)
    mat3[3] mRegion;
    
    for (int i = 0; i < 3; i++)
        mRegion[i] = mat3(
        	region[0][i], region[1][i], region[2][i],
        	region[3][i], region[4][i], region[5][i],
        	region[6][i], region[7][i], region[8][i]
    	);
    
    return mRegion;
}

// Convolve a texture with kernel
// kernel : kernel used for convolution
// sampler : texture sampler
// uv : current coordinates on sampler
vec3 convolution(mat3 kernel, sampler2D sampler, vec2 uv)
{
    vec3 fragment;
    
    // Extract a 3x3 region centered in uv
    mat3[3] region = region3x3(sampler, uv);
    
    // for each color channel of region
    for (int i = 0; i < 3; i++)
    {
        // get region channel
        mat3 rc = region[i];
        // component wise multiplication of kernel by region channel
        mat3 c = matrixCompMult(kernel, rc);
        // add each component of matrix
        float r = c[0][0] + c[1][0] + c[2][0]
                + c[0][1] + c[1][1] + c[2][1]
                + c[0][2] + c[1][2] + c[2][2];
        
        // for fragment at channel i, set result
        fragment[i] = r;
    }
    
    return fragment;    
}


void main()
{
   // current position
    vec2 tex_coords = gl_FragCoord.xy  / (textureSize(tex_drawing, 0).x) ;


    mat3 grad_kernel =  mat3(0, 1, 0, 1, -4, 1, 0, 1, 0);
    // Convolve kernel with texture
    vec3 col = convolution( grad_kernel, tex_drawing, tex_coords);
    
    // Output to screen
    outColor = vec4(col, 1.0);



  //  outColor =  texture(tex_drawing, tex_coords);
}