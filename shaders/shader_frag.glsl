#version 450 core

// Global variables for lighting calculations.
layout(location = 1) uniform vec3 viewPos;
layout(location = 2) uniform sampler2D tex_drawing;

//layout(location = 3) uniform mat4 lightMVP;
//layout(location = 4) uniform vec3 lightPos = vec3(3, 3, 3);
  
// Output for on-screen color.
layout(location = 0) out vec4 outColor;

// Interpolated output data from vertex shader.
in vec3 fragPos; // World-space position
in vec3 fragNormal; // World-space normal
in vec4 gl_FragCoord; // window relative fragment coordinates

void main()
{
    // Output the normal as color.
  // vec3 lightDir = normalize(lightPos - fragPos);
    vec2 tex_coords = gl_FragCoord.xy / (textureSize(tex_drawing, 0).x) ;
    outColor =  texture(tex_drawing, tex_coords);
    //outColor = vec4(vec3(max(dot(fragNormal, lightDir), 0.0)), 1.0);
}