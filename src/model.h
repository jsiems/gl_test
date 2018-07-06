
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

#include <stb_image.h>              //used to load images
#include <cglm/cglm.h>
#include <glad/glad.h>              //defines opengl functions, etc


#include "wfc.h"
#include "shader.h"
#include "camera.h"

#define FPV 8

// TODO: cache textures incase other models use the same texture.
// This will increase loading times. Maybe create a texture manager
// file

// TODO: Probably should do dynamic allocation for the filenames
// in case i ever have very long filenames

struct Model {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int texture;
    unsigned int texture_spec_map;
    unsigned int num_verts;
};

//will load all textures, vertex data, buffer the data
void initializeModel(struct Model *model, char *modelname);

// draws model at each position sent into the function
void drawModel(struct Model *model, struct Shader *shader, 
               int num_positions, vec3 *positions);

void destroyModel(struct Model *model);

#endif

