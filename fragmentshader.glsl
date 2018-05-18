
#version 330 core

out vec4 FragColor;

//IF THIS INPUT NAME IS NOT THE EXACT SAME AS THE OUTPUT NAME, 
//  INPUT WILL NOT BE RECEIVE!!!! 
in vec2 tex_coord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float mixture;

void main() {
   FragColor = mix(texture(texture0, tex_coord), texture(texture1, tex_coord), mixture);
}
