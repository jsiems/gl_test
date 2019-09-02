
#include "text.h"

struct Character chars[128];
struct Shader shader_g;
unsigned int vao = 0, vbo = 0;
int initialized = 0;

void initTextRenderer() {
    // initialize shader
    if(!initializeShader(&shader_g, "shaders/text_vs.glsl", "shaders/text_fs.glsl")) {
        printf("Error initializing text shaders\n");
        exit(1);
    }

    // tell the sprite_shader_g which texture to use for which uniform
    glUseProgram(shader_g.id);
    setInt(&shader_g, "text", 0);

    mat4 projection;
    glm_ortho(0.0, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -1.0, 1.0, projection);
    setMat4(&shader_g, "projection", projection);

    // more opengl initialization
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        printf("FREETYPE ERROR: could not init freetype library\n");
        exit(1);
    }

    FT_Face face;
    if(FT_New_Face(ft, "C:/Windows/Fonts/arial.ttf", 0, &face)) {
        printf("FREETYPE ERROR: Failed to load font\n");
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // avoid alignment issues
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load all 128 ascii characters
    for(int i = 0; i < 128; i ++) {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            printf("FREETYPE ERROR: Failed to load glyph for i=%d\n", i);
            chars[i].tex_id = 0;
            continue;
        }

        // generate texture
        unsigned int tex_id;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // store for later use
        chars[i].tex_id = tex_id;
        chars[i].width = face->glyph->bitmap.width;
        chars[i].height = face->glyph->bitmap.rows;
        chars[i].bearing_x = face->glyph->bitmap_left;
        chars[i].bearing_y = face->glyph->bitmap_top;
        chars[i].advance = face->glyph->advance.x;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, 0, GL_DYNAMIC_DRAW); // pos and tex coordinates
    // position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    // texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized = 1;
}

void renderText(char *string, float x, float y, float scale, vec3 color) {

    if(!initialized) {
        printf("Render Text ERROR: Cannot render, not initialized\n");
        return;
    }

    y = SCREEN_HEIGHT - y;

    glUseProgram(shader_g.id);
    setVec3(&shader_g, "text_color", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    // iterate through the string
    int i = 0;
    while(string[i] != 0) {
        struct Character c = chars[(int)string[i]];

        // skip characters that could not be loaded
        if(c.tex_id == -1) {
            i ++;
            continue;
        }

        float xpos = x + c.bearing_x * scale;
        float ypos = y - (c.height - c.bearing_y) * scale;

        float w = c.width * scale;
        float h = c.height * scale;

        // update VBO for each character
        float verts[6][4] = {
            {xpos,     ypos + h, 0.0, 0.0},
            {xpos,     ypos,     0.0, 1.0},
            {xpos + w, ypos,     1.0, 1.0},

            {xpos,     ypos + h, 0.0, 0.0},
            {xpos + w, ypos,     1.0, 1.0},
            {xpos + w, ypos + h, 1.0, 0.0}
        };

        glBindTexture(GL_TEXTURE_2D, c.tex_id);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += (c.advance >> 6) * scale;

        i ++;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}