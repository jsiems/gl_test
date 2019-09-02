
#version 330 core

// Uniforms
uniform sampler2D text;
uniform vec3 text_color;

// ***** inputs / outputs *****
out vec4 color;
in vec2 tex_coords;

void main() {
    // for more on alpha stuff, see: opengl blending tutorial on learnopengl.com
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coords).r);
    vec4 tex_color = vec4(text_color, 1.0) * sampled;
    if(tex_color.a < 0.1) {
        discard;
    }
    color = tex_color;
}
