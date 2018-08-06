
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


#include "wfc.h"
#include "shader.h"
#include "camera.h"
#include "texman.h"

#define FPV 8

// TODO: cache textures incase other models use the same texture.
// This will increase loading times. Maybe create a texture manager
// file

// TODO: Probably should do dynamic allocation for the filenames
// in case i ever have very long filenames

struct Model {
    struct Mesh *meshes;
    int num_meshes;
};

struct Mesh {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int num_verts;
    unsigned int texture;
    unsigned int texture_spec_map;
};

//will load all textures, vertex data, buffer the data
void initializeModel(struct Model *model, struct TexMan *texman, char *modelname);

// draws model at each position sent into the function
void drawModels(struct Model *model, struct Shader *shader, 
               int amount, vec3 *positions, vec3 *rotations, vec3 *scales);

void destroyModel(struct Model *model);

#endif

