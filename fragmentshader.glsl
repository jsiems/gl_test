
#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float mixture;

void main() {
   FragColor = mix(texture(texture0, texCoord), texture(texture1, texCoord), mixture) * vec4(ourColor, 1.0f);
}
