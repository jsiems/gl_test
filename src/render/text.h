// File: text.h
// Desc: contains functions for writing text to screen

#ifndef TEXT_H
#define TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <cglm/cglm.h>

#include "render/shader.h"
#include "const.h"

// see: https://learnopengl.com/In-Practice/Text-Rendering

struct Character {
    unsigned int tex_id;     // id handle of glyph
    int width;      // size of glyph
    int height;
    int bearing_x;  // offset from baseline to left/top of glyph
    int bearing_y;
    int advance;    // horizontal offset to advance to next glyph
};

void initTextRenderer();

void renderText(char *string, float x, float y, float scale, vec3 color);

#endif