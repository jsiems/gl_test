
#include "shader.h"

uint8_t initializeShader(struct shader *shdr, const char *vertex_filename, const char *fragment_filename) {
    FILE *vertex_file;      //files to be read
    FILE *fragment_file;    //intellisense freaks out here but it is fine
    char *vertex_source;        //source strings from files
    char *fragment_source;
    uint32_t vertex_shader;     //id's generated by opengl
    uint32_t fragment_shader;
    uint32_t success;                
    char info_log[512];
    uint32_t file_size;

    
    vertex_file = fopen(vertex_filename, "rb");
    fragment_file = fopen(fragment_filename, "rb");

    if(vertex_file == NULL) {
        perror("Error opening vertex file ");
        return 0;
    }
    if(fragment_file == NULL) {
        perror("Error opening fragment file ");
        return 0;
    }

    //read in the vertex shader
    fseek(vertex_file, 0L, SEEK_END);
    file_size = ftell(vertex_file);
    rewind(vertex_file);
    vertex_source = malloc(file_size + 1);
    if(vertex_source == NULL) {
        perror("Error allocating vertex string memory ");
        return 0;
    }
    if(fread(vertex_source, file_size, 1, vertex_file) != 1) {
        perror("Error reading entire vertex file ");
        return 0;
    }
    vertex_source[file_size] = '\0';

    //read in the fragment shader
    fseek(fragment_file, 0L, SEEK_END);
    file_size = ftell(fragment_file);
    rewind(fragment_file);
    fragment_source = malloc(file_size + 1);
    if(fragment_source == NULL) {
        perror("Error allocating fragment string memory ");
        return 0;
    }
    if(fread(fragment_source, file_size, 1, fragment_file) != 1) {
        perror("Error reading entire fragment file ");
        return 0;
    }
    fragment_source[file_size] = '\0';

    //Vertex shader compilation
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const char **)&vertex_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex_shader, GL_COMPILE_STATUS, NULL, info_log);
        printf("ERROR: vertex shader compilation failed\n\t%s\n", info_log);
        return -1;
    }

    //fragment shader compilation
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const char **)&fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment_shader, GL_COMPILE_STATUS, NULL, info_log);
        printf("ERROR: fragment shader compilation failed\n\t%s\n", info_log);
        return -1;
    }

    //SHADER program linkage
    shdr->id = glCreateProgram();
    glAttachShader(shdr->id, vertex_shader);
    glAttachShader(shdr->id, fragment_shader);
    glLinkProgram(shdr->id);
    glGetProgramiv(shdr->id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shdr->id, 512, NULL, info_log);
        printf("ERROR: program 1 linkage failed\n\t%s\n", info_log);
        return -1;
    }

    //vertex and fragment shaders deleted after linking to shader program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    //free the dynamically allocated character arrays
    free(vertex_source);
    free(fragment_source);

    //close the files opened
    fclose(vertex_file);
    fclose(fragment_file);

    return 1;
}
