
#include "model.h"

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
    
    // models/modelname/modelname.vrt, modelname.obj
    char vrtfn[100];
    strcpy(vrtfn, basename);
    strcat(vrtfn, ".vrt");
    char objfn[100];
    strcpy(objfn, basename);
    strcat(objfn, ".obj");

    // check for and convert .obj to .vrt if .vrt is older or nonexistent
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

    // read total number of meshes
    int num_meshes;
    fread(&num_meshes, sizeof(num_meshes), 1, file);
    model->num_meshes = num_meshes;
    model->meshes = malloc(sizeof(struct Mesh) * num_meshes);
    if(model->meshes == 0) {
        printf("ERROR allocating mem for model meshes for model %s\n", modelname);
        exit(1);
    }

    for(int i = 0; i < num_meshes; i ++) {
        struct Mesh newmesh;

        // read texture name, convert to 'textures/name.png'
        int name_len;
        fread(&name_len, sizeof(name_len), 1, file);
        char *name = malloc(name_len + 1);
        fread(name, sizeof(*name), name_len, file);
        texname[name_len] = '\0';
        char *texname = malloc(name_len + 20);
        // load texture
        strcpy(texname, "textures/\0");
        strcat(texname, name);
        strcat(texname, ".png\0");
        newmesh.texture = loadTexture(texname);
        // TODO: allow spec map to be passed in with function
        //       or a part of the .vrt
        // load spec map
        strcpy(texname, "textures/\0");
        strcat(texname, name);
        strcat(texname, "_spec.png\0");
        newmesh.texture_spec_map = loadTexture(texname);
        // free texture name strings
        free(name);
        free(texname);

        // read number of vertices
        int num_verts;
        fread(num_verts, sizeof(num_verts), 1, file);
        newmesh.num_verts = num_verts;

        // read vertex data
        float *verts = malloc(num_verts * FPV * sizeof(*verts));
        fread(verts, sizeof(*verts), FPV * num_verts, file);

        // generate buffer and array objects
        int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBinderBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, num_verts * FPV * sizeof(float), verts, GL_STATIC_DRAW);

        // vertex buffer attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)0);
        glEnableVertexAttribPointer(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)(6 * sizeof(float)));

        glBindVertexArray(0);

        free(verts);
        newmesh.VAO = VAO;
        newmesh.VBO = VBO;

        model->meshes[i] = newmesh;
    }
    fclose(file);
}

void drawModels(struct Model *model, struct Shader *shader, int amount, vec3 *positions, vec3 *rotations, vec3 *scales) {
    for(int i = 0; i < model->num_meshes; i ++) {
        drawMeshes(&model->meshes[i], shader, amount, positions, rotations, scales);
    }
}

// draws model at each position sent into the function
void drawMeshes(struct Mesh *mesh, struct Shader *shader, 
               int amount, vec3 *positions, vec3 *rotations, vec3 *scales) {
    //bind active textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh->texture_spec_map);

    //Configure uniforms and draw vertices for model
    glUseProgram(shader->id);
    glBindVertexArray(mesh->VAO);

    // TODO: Move this to be contained in .mtl file
    float shininess = 32.0f;
    setFloat(shader, "material.shininess", shininess);

    for(int i = 0; i < amount; i ++) {
        mat4 transformation;
        glm_translate_make(transformation, positions[i]);
        // not sure this is a great way to accomplish rotation
        vec3 norm;
        glm_vec_normalize_to(rotations[i], norm);
        glm_rotate(transformation, glm_vec_norm(rotations[i]), norm);
        glm_scale(transformation, scales[i]);
        setMat4(shader, "model", transformation);
        glDrawArrays(GL_TRIANGLES, 0, mesh->num_verts);
    }

    glBindVertexArray(0);

    return;
}

void destroyModel(struct Model *model) {
    //anything else to destroy?
    glDeleteVertexArrays(1, &model->VAO);
    glDeleteBuffers(1, &model->VBO);

    free(model->meshes);

    return;
}
