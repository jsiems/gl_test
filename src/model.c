
#include "model.h"

void initializeModel(struct Model *model, struct TexMan *texman, char *modelname) {

    // basename becomes models/modelname/modelname
    // add extensions to basename to open files
    int mn_length = strlen(modelname);
    char *basename = malloc(sizeof(*basename) * (2 * mn_length + 9));
    strcpy(basename, "models/\0");
    strcat(basename, modelname);
    strcat(basename, "/\0");
    strcat(basename, modelname);
    
    // models/modelname/modelname.vrt, modelname.obj
    int bn_length = strlen(basename);
    char *vrtfn = malloc(sizeof(*vrtfn) * (bn_length + 5));
    strcpy(vrtfn, basename);
    strcat(vrtfn, ".vrt\0");
    char *objfn = malloc(sizeof(*objfn) * (bn_length + 5));
    strcpy(objfn, basename);
    strcat(objfn, ".obj\0");
    
    free(basename);

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

    free(objfn);

    FILE *file;
    file = fopen(vrtfn, "rb");
    if(!file) {
        printf("ERROR reading vertices: opening %s\n", vrtfn);
        perror("");
        exit(1);
    }


    free(vrtfn);

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

        // read texture name and load texture
        int name_len;
        fread(&name_len, sizeof(name_len), 1, file);
        char *name = malloc(name_len + 1);
        if( fread(name, sizeof(*name), name_len, file) != name_len) {
            printf("error reading texture name \n");
            exit(1);
        }
        name[name_len] = '\0';
        newmesh.texture = getTextureId(texman, name);

        // check if spec map available
        // read spec map name and load spec map (if it exists)
        // TODO: If spec_map size is 0, change to check for name_spec.png
        // TODO: Create texture not found texture
        char *specname = malloc(name_len + 7);
        strcpy(specname, name);
        specname[name_len] = '\0';
        strcat(specname, "_spec\0");
        newmesh.texture_spec_map = getTextureId(texman, specname);
        // free texture name strings
        free(name);
        free(specname);

        // read number of vertices
        int num_verts;
        fread(&num_verts, sizeof(num_verts), 1, file);
        newmesh.num_verts = num_verts;

        // read vertex data
        float *verts = malloc(num_verts * FPV * sizeof(*verts));
        fread(verts, sizeof(*verts), FPV * num_verts, file);

        // generate buffer and array objects
        unsigned int VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, num_verts * FPV * sizeof(float), verts, GL_STATIC_DRAW);

        // vertex buffer attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, FPV * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        free(verts);
        newmesh.VAO = VAO;
        newmesh.VBO = VBO;

        model->meshes[i] = newmesh;
    }
    fclose(file);
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

void drawModels(struct Model *model, struct Shader *shader, int amount, vec3 *positions, vec3 *rotations, vec3 *scales) {
    for(int i = 0; i < model->num_meshes; i ++) {
        drawMeshes(&model->meshes[i], shader, amount, positions, rotations, scales);
    }
}


void destroyModel(struct Model *model) {
    //anything else to destroy?

    for(int i = 0; i < model->num_meshes; i ++) {
        //struct Mesh current = model->meshes[i];
        //glDeleteVertexArrays(1, &current->VAO);
        //glDeleteBuffers(1, &current->VBO);
    }

    free(model->meshes);

    return;
}
