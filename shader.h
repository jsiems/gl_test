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

//sets the value of a uniform
//  MUST BE CALLED AFTER SHADER IS BINDED
//  true. location is gotten using ID, but when setting, you only use 
//  location, not the id. This means whichever program is bound will be set
//
//  to use with single value, pass pointer to value casted to void pointer
//  e.g. float num = 1.2; setUniform(shdr, "nam", type, 1, (void *)&num);
//
//  to use with mat2, just cast to void pointer
//  float num[2][2] = {{1.0, 0.0},{0.0, 1.0}}; setUniform(shdr, "nam", type, 2, (void *)num);
//
//  to use with mat3 (or mat4 probably), same as previous
//  mat3 num = {{...},{...},{...}}; setUniform(shdr, "nam", type, 3, (void *)num);
void setUniform(struct shader *shdr, const char *name, enum uniform_data_type data_type, uint8_t size, void *data);

#endif
