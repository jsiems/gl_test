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

//returns 1 is successful, 0 if not
uint8_t initializeShader(struct shader *shdr, const char *vertex_filename, const char *fragment_filename);

#endif