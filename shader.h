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

void setInt(struct shader *shdr, const char *name, int data);

void setFloat(struct shader *shdr, const char *name, float data);

void setVec2(struct shader *shdr, const char *name, float data[2]);

void setVec3(struct shader *shdr, const char *name, float data[3]);

void setMat2(struct shader *shdr, const char *name, float data[2][2]);

void setMat3(struct shader *shdr, const char *name, float data[3][3]);

void setMat4(struct shader *shdr, const char *name, float data[4][4]);

#endif
