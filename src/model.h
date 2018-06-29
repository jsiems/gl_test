
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
#include <sys/stat.h>
#include <cglm/cglm.h>

#include "wfc.h"

// TODO: cache textures incase other models use the same texture.
// This will increase loading times. Maybe create a texture manager
// file

struct Model {
    
};

//will load all textures, vertex data, buffer the data
void initializeModel(struct Model *model, char *modelname) {

    // basename becomes models/modelname/modelname
    // add extensions to basename to open files
    char basename[100] = "models/\0";
    strcat(basename, modelname);
    strcat(basename, "/\0");
    strcat(basename, modelname);

    float *verts = getVertices(basename);

    // THIS MIGHT MESS THINGS UP WITH OPENGL
    // not sure how it will like freeing the memory for 
    // the vertex data... cause it might already be sent down
    // but also, it might not be sent down and it might 
    // be relying on the data we have.
    free(verts);
}


// Load model vertices
// basename is path to file without any extensions
// ALLOCATES MEMORY FOR RETURN POINTER
// don't forget to free the memory afterwards
float *getVertices(char *basename) {
    char vrtfn[100];
    strcpy(vrtfn, basename);
    strcat(vrtfn, ".vrt");
    char objfn[100];
    strcpy(objfn, basename);
    strcat(objfn, ".vrt");

    // do check for file
    // maybe don't do this and make the user of the engine keep track
    // would be faster... but I would ALWAYS forget to convert it.
    struct stat result;
    if(stat(vrtfn, &result) == 0) {
        printf("file %s exists\n", vrtfn);
        struct timespec vrt_time = result.st_mtim;

        if(stat(objfn, &result) == 0) {
            printf("other %s exists\n", objfn);
            struct timespec obj_time = result.st_mtim;
            if(obj_time.tv_sec <= vrt_time.tv_sec) {
                printf("obj newer, generating vrt\n");
                convertWavefront(objfn);
            }
            else {
                printf("vrt newer, doing nothing\n");
            }
        }
        else {
            // obj file does not exist, raise an error
            // if we can find .vrt but not .obj,
            // do we really care?
            //printf("ERROR locating %s\n", objfn);
            //printf("File does not exist?\n");
            //exit(1);
        }
    }
    else {
        printf("The file %s does not exist\n", filename);
        strcpy(basename, filename);
        strcat(filename, ".obj\0");
        convertWavefront(filename);
    }

    FILE *file;
    file = fopen(vrtfn, "rb");
    if(!file) {
        printf("ERROR opening %s\n", vrtfn);
        perror("");
        exit(1);
    }

    // read total number of vertices from input file
    int num_verts;
    fread(&num_verts, sizeof(num_verts), 1, file);
    float *verts = malloc(num_verts * FPV * sizeof(*verts));
    if(!verts) {
        printf("ERROR: could not allocate memory for verts\n");
        exit(1);
    }

    // load all data into verts array
    for(int i = 0; i < num_verts; i ++) {
        fread(verts + i * FPV, sizeof(*verts), FPV, file);
    }

    fclose(file);

    return verts;
}

#endif

