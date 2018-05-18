
#include <stdio.h>
#include <math.h>
#include <glad/glad.h>              //defines opengl functions, etc
#include <GLFW/glfw3.h>             //used for window and input
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>              //used to load images
#include <cglm/cglm.h>              //used for maths

#include "shader.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void glfw_error_callback(int code, const char *err_str);
GLFWwindow *initializeWindow();

float mixture = 0.0f;
int up_pressed = 0;
int down_pressed = 0;

int main() {
    //initialize window
    GLFWwindow *window = initializeWindow();
    //load the opengl library
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initiate GLAD\n");
        return -1;
    }
    //initialize the shaders
    struct shader shader_program;
    if(!initializeShader(&shader_program, "vertexshader.glsl", "fragmentshader.glsl")) {
        printf("Error initializing shader_program\n");
        return -1;
    }

    vec4 vec = {1.0f, 0.0f, 0.0f, 1.0f};
    mat4 trans;
    //creates new translation matrix 
    glm_translate_make(trans, (vec3){1.0f, 1.0f, 0.0f});
    //multiplies translations by vec, stores in vec
    glm_mat4_mulv(trans, vec, vec);
    printf("x: %f | y: %f | z: %f\n", vec[0], vec[1], vec[2]);


    //load textures
    int image_width, image_height, nr_channels;
    unsigned char *image_data;
    unsigned int texture0, texture1;       //texture id given by opengl

    //box
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image data
    stbi_set_flip_vertically_on_load(1);
    image_data = stbi_load("container.jpg", &image_width, &image_height, &nr_channels, 0);
    if(image_data != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("Failed to load texture container\n");
        return -1;
    }
    stbi_image_free(image_data);

    //happy face
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image data
    image_data = stbi_load("awesomeface.png", &image_width, &image_height, &nr_channels, 0);
    if(image_data != NULL) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        printf("Failed to load texture awesome face\n");
        return -1;
    }
    stbi_image_free(image_data);


    //-------------vertex data and buffers

    float vertices[] = {
        //positions           //colors            //texture coordinates
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //vertex array object, stores vertex buffer object and vertex attribute data
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //bind the vertex array object
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glUseProgram(shader_program.id);
    glUniform1i(glGetUniformLocation(shader_program.id, "texture0"), 0);
    glUniform1i(glGetUniformLocation(shader_program.id, "texture1"), 1);

    //Main loop
    while(!glfwWindowShouldClose(window)) {
        //process inputs
        processInput(window);

        //rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program.id);
        glUniform1f(glGetUniformLocation(shader_program.id, "mixture"), mixture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

//should definitely use key callback for this
//  keycallback insures that will we handle the input
//  even if they release the key before we process the input
//  in the loop
void processInput(GLFWwindow *window) {
    int escape = glfwGetKey(window, GLFW_KEY_ESCAPE);
    int up = glfwGetKey(window, GLFW_KEY_UP);
    int down = glfwGetKey(window, GLFW_KEY_DOWN);
    if(escape == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1U);
    if(up == GLFW_PRESS && !up_pressed) {
        mixture += 0.1f;
        up_pressed = 1;
    }
    else if(up == GLFW_RELEASE && up_pressed) {
        up_pressed = 0;
    }
    if(down == GLFW_PRESS && !down_pressed) {
        mixture -= 0.1f;
        down_pressed = 1;
    }
    else if(down == GLFW_RELEASE && down_pressed) {
        down_pressed = 0;
    }
    if(mixture < 0.0f) {
        mixture = 0.0f;
    }
    if(mixture > 1.0f) {
        mixture = 1.0f;
    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void glfw_error_callback(int code, const char *err_str) {
    printf("GLFW error: \n\tcode: 0x%x\n\t%s\n", code, err_str);
}

GLFWwindow *initializeWindow() {
    glfwSetErrorCallback(glfw_error_callback);

    if(!glfwInit()) {
        printf("error initializing GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "TITLE", NULL, NULL);

    if(window == NULL) {
        printf("Error creating window\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}
