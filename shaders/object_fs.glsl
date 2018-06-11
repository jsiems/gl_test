
#version 330 core

out vec4 FragColor;

in vec3 normal_vec;
in vec3 frag_pos;
in vec2 tex_coords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform Material material;

struct Light {
    vec3 position;
    vec3 direction;
    float cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;

uniform vec3 view_pos;

void main() {
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords));

    vec3 light_dir = normalize(light.position - frag_pos);
    float theta = dot(light_dir, normalize(-light.direction));

    if(theta > light.cutoff) {
        float distance = length(light.position - frag_pos);
        float attenuation = 1.0 / (light.constant + light.linear * distance +
                            light.quadratic * (distance * distance));
    
        vec3 norm = normalize(normal_vec);
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords));

        vec3 view_dir = normalize(view_pos - frag_pos);
        vec3 reflect_dir = reflect(-light_dir, norm);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

        //adding attenuation to ambient here makes light darker inside spolight than
        //  outside at large distances (cause it's not added in else statement)
        FragColor = vec4(attenuation * (ambient + diffuse + specular), 1.0);
    }
    else {
        FragColor = vec4(ambient, 1.0);
    }
}
