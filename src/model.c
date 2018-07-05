
#include "model.h"

// Load model vertices
// basename is path to file without any extensions
// ALLOCATES MEMORY FOR RETURN POINTER
// don't forget to free the memory afterwards
int getVertices(char *basename, float **verts) {
    char vrtfn[100];
    strcpy(vrtfn, basename);
    strcat(vrtfn, ".vrt");
    char objfn[100];
    strcpy(objfn, basename);
    strcat(objfn, ".obj");

    // do check for file
    // maybe don't do this and make the user of the engine keep track
    // would be faster... but I would ALWAYS forget to convert it.
    struct stat result;
    if(stat(vrtfn, &result) == 0) {
        time_t vrt_time = result.st_mtime;

        if(stat(objfn, &result) == 0) {
            time_t obj_time = result.st_mtime;
            if(obj_time >= vrt_time) {
                convertWavefront(objfn);
            }
        }
    }
    else {
        convertWavefront(objfn);
    }

    FILE *file;
    file = fopen(vrtfn, "rb");
    if(!file) {
        printf("ERROR reading vertices: opening %s\n", vrtfn);
        perror("");
        exit(1);
    }

    // read total number of vertices from input file
    int num_verts;
    fread(&num_verts, sizeof(num_verts), 1, file);
    *verts = malloc(num_verts * FPV * sizeof(*verts));
    if(!*verts) {
        printf("ERROR: could not allocate memory for verts\n");
        exit(1);
    }

    // load all data into verts array
    for(int i = 0; i < num_verts; i ++) {
        fread(*verts + i * FPV, sizeof(**verts), FPV, file);
    }

    fclose(file);

    return num_verts;
}

unsigned int loadTexture(char * name) {
    unsigned char *image_data;
    int image_width, image_height, nr_channels;
    unsigned int texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    image_data = stbi_load(name, &image_width, &image_height, &nr_channels, 0);
    if(image_data == NULL) {
        printf("Failed to load texture %s\n", name);
        return -1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    return texture;
}

void initializeModel(struct Model *model, char *modelname) {

    // basename becomes models/modelname/modelname
    // add extensions to basename to open files
    char basename[100] = "models/\0";
    strcat(basename, modelname);
    strcat(basename, "/\0");
    strcat(basename, modelname);

    // ***** load shaders *****
    char vs[100], fs[100];
    strcpy(vs, basename);
    strcpy(fs, basename);
    strcat(vs, "_vs.glsl\0");
    strcat(fs, "_fs.glsl\0");
    if(!initializeShader(&model->shader, vs, fs)) {
        printf("Error initializing %s shaders\n", modelname);
        exit(1);
    }


    // ***** Load textures *****
    char imgfn[100];
    strcpy(imgfn, basename);
    strcat(imgfn, ".png");
    model->texture = loadTexture(imgfn);
    strcpy(imgfn, basename);
    strcat(imgfn, "_spec.png");
    model->texture_spec_map = loadTexture(imgfn);

    // tell the shader which texture to use for which uniform
    glUseProgram(model->shader.id);
    setInt(&model->shader, "material.diffuse", 0);
    setInt(&model->shader, "material.specular", 1);


    // ***** Load vertices *****
    // don't forget to free this memory
    float *vertices = 0;
    model->num_verts = getVertices(basename, &vertices);
    
    for(int i = 0; i < model->num_verts; i ++) {
        printf("verts[%d] textures: %f, %f\n", i, vertices[i * FPV + 6], vertices[i * FPV + 7]);
    }

    glGenVertexArrays(1, &model->VAO);
    glGenBuffers(1, &model->VBO);
    glBindVertexArray(model->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, model->VBO);
    glBufferData(GL_ARRAY_BUFFER, model->num_verts * FPV * sizeof(float), vertices, GL_STATIC_DRAW);

    // vertex buffer attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    
    free(vertices);
}

// draws model at each position sent into the function
// TODO: Change this so you can send scale and rotation in also
void drawModel(struct Model *model, int num_positions, vec3 *positions,
               int num_lights, vec3 *light_positions, 
               mat4 *view, mat4 *projection, struct Camera *cam,
               int flashlight_on) {
    //bind active textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, model->texture_spec_map);

    //Configure uniforms and draw vertices for model
    glUseProgram(model->shader.id);
    glBindVertexArray(model->VAO);

    setMat4(&model->shader, "view", *view);
    setMat4(&model->shader, "projection", *projection);
    setVec3(&model->shader, "view_pos", cam->position);

    //sets the objects color and the color of the light hitting the object
    float shininess = 32.0f;
    setFloat(&model->shader, "material.shininess", shininess);

    //light colors n such
    vec3 light_color = {1.0f, 1.0f, 1.0f};
    vec3 diffuse_color;
    glm_vec_mul(light_color, (vec3){0.5f, 0.5f, 0.5f}, diffuse_color);
    vec3 ambient_color;
    glm_vec_mul(diffuse_color, (vec3){0.2f, 0.2f, 0.2f}, ambient_color);
    setVec3(&model->shader, "dir_light.direction", (vec3){-0.2f, -1.0f, -0.3f});
    setVec3(&model->shader, "dir_light.ambient", ambient_color);
    setVec3(&model->shader, "dir_light.diffuse", diffuse_color);
    setVec3(&model->shader, "dir_light.specular", (vec3){1.0f, 1.0f, 1.0f});

    //set uniforms for point lights
    float light_const = 1.0f, light_lin = 0.09f, light_quad = 0.032f;
    for(int i = 0; i < num_lights; i ++) {
        char uniform[] = "point_lights[i].";
        uniform[13] = i + '0';
        char uniform_str[30] = "";

        //uniform_str = "point_lights[i].position"
        strcpy(uniform_str, uniform); strcat(uniform_str, "position");
        setVec3(&model->shader, uniform_str, light_positions[i]);

        strcpy(uniform_str, uniform); strcat(uniform_str, "ambient");
        setVec3(&model->shader, uniform_str, ambient_color);

        strcpy(uniform_str, uniform); strcat(uniform_str, "diffuse");
        setVec3(&model->shader, uniform_str, diffuse_color);

        strcpy(uniform_str, uniform); strcat(uniform_str, "specular");
        setVec3(&model->shader, uniform_str, (vec3){1.0f, 1.0f, 1.0f});

        strcpy(uniform_str, uniform); strcat(uniform_str, "constant");
        setFloat(&model->shader, uniform_str, light_const);

        strcpy(uniform_str, uniform); strcat(uniform_str, "linear");
        setFloat(&model->shader, uniform_str, light_lin);

        strcpy(uniform_str, uniform); strcat(uniform_str, "quadratic");
        setFloat(&model->shader, uniform_str, light_quad);
    }

    //uniforms for spotlight
    float cutoff = cos(degToRad(12.5f));
    float outer_cutoff = cos(degToRad(17.f));
    setVec3(&model->shader, "spot_light.position", cam->position);
    setVec3(&model->shader, "spot_light.direction", cam->front);
    setFloat(&model->shader, "spot_light.constant", light_const);
    setFloat(&model->shader, "spot_light.linear", light_lin);
    setFloat(&model->shader, "spot_light.quadratic", light_quad);
    setVec3(&model->shader, "spot_light.diffuse", diffuse_color);
    setVec3(&model->shader, "spot_light.specular", (vec3){1.0f, 1.0f, 1.0f});
    setFloat(&model->shader, "spot_light.cutoff", cutoff);
    setFloat(&model->shader, "spot_light.outer_cutoff", outer_cutoff);
    setInt(&model->shader, "spot_light.on", flashlight_on);

    for(int i = 0; i < num_positions; i ++) {
        mat4 pos;
        glm_translate_make(pos, positions[i]);
        glm_rotate(pos, i * degToRad(20.0f), (vec3){0.5f, 1.0f, 0.0f});
        setMat4(&model->shader, "model", pos);
        glDrawArrays(GL_TRIANGLES, 0, model->num_verts);
    }

    glBindVertexArray(0);

    return;
}

void destroyModel(struct Model *model) {
    //anything else to destroy?
    glDeleteVertexArrays(1, &model->VAO);
    glDeleteBuffers(1, &model->VBO);

    return;
}