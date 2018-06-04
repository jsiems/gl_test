#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <glad/glad.h>

struct shader {
    uint32_t id;
};

enum uniform_data_type {
    uniform_int,
    uniform_float,   //used for vectors
    uniform_matrix   //matrix assumes float type
};

//returns 1 is successful, 0 if not
uint8_t initializeShader(struct shader *shdr, const char *vertex_filename, const char *fragment_filename);

//returns uniform location, exits if uniform cannot be located
uint8_t getUniformLocation(struct shader *shdr, const char *name);

//sets the value of a uniform
void setUniform(struct shader *shdr, const char *name, enum data_type, uint8_t size, void *data);

#endif
