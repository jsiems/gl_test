
// controls all aspects of a model,
// from loading the vertices, normals
// texture coordinates, textures, 
// to drawing the model, loading shaders,
// buffering the data, etc... this will
// do it all

#ifndef MODEL_H
#define MODEL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

#include <cglm/cglm.h>
#include <glad/glad.h>              //defines opengl functions, etc


#include "helper/wfc.h"
#include "render/shader.h"
#include "render/camera.h"
#include "render/texman.h"

#define FPV 8

struct Model {
    struct Mesh *meshes;
    int num_meshes;
};

struct Mesh {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int num_verts;
    unsigned int texture;           // int points to texture in texman
    unsigned int texture_spec_map;  // same
};

//will load all textures, vertex data, buffer the data
void initializeModel(struct Model *model, struct TexMan *texman, char *modelname);

// draws model at each position sent into the function
void drawModels(struct Model *model, struct Shader *shader, 
               int amount, vec3 *positions, vec3 *rotations, vec3 *scales);

void destroyModel(struct Model *model);

#endif

