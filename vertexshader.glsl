
#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord_in;

//OUTPUT MUST HAVE THE ****EXACT**** SAME NAME AS INPUT TO FRAGMENT SHADER
out vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
   gl_Position = projection * view * model * vec4(pos, 1.0);
   tex_coord = vec2(tex_coord_in.x, tex_coord_in.y);
}

