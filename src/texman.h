#ifndef TEXMAN_H
#define TEXMAN_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include <stb_image.h>
#include <glad/glad.h>

// single linked list of textures
struct Texture {
    struct Texture *next;
    unsigned int id;
    char *name;
};

struct TexMan {
    struct Texture *head;
    struct Texture *tail;
};

void initTexMan(struct TexMan *texman);

unsigned int getTextureId(struct TexMan *texman, char *name);

void destroyTexMan(struct TexMan *texman);

#endif
