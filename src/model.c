
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

    // ***** Load textures *****
    char imgfn[100];
    strcpy(imgfn, basename);
    strcat(imgfn, ".png");
    model->texture = loadTexture(imgfn);
    strcpy(imgfn, basename);
    strcat(imgfn, "_spec.png");
    model->texture_spec_map = loadTexture(imgfn);

    // ***** Load vertices *****
    // don't forget to free this memory
    float *vertices = 0;
    model->num_verts = getVertices(basename, &vertices);
    
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
void drawModel(struct Model *model, struct Shader *shader, 
               int num_positions, vec3 *positions) {
    //bind active textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, model->texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, model->texture_spec_map);

    //Configure uniforms and draw vertices for model
    glUseProgram(shader->id);
    glBindVertexArray(model->VAO);

    // TODO: Move this to be contained in .mtl file
    float shininess = 32.0f;
    setFloat(shader, "material.shininess", shininess);

    for(int i = 0; i < num_positions; i ++) {
        mat4 pos;
        glm_translate_make(pos, positions[i]);
        glm_rotate(pos, i * degToRad(20.0f), (vec3){0.5f, 1.0f, 0.0f});
        setMat4(shader, "model", pos);
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