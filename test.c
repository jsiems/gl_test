
#include <stdio.h>
#include <math.h>
#include <glad/glad.h>              //defines opengl functions, etc
#include <GLFW/glfw3.h>             //used for window and input
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>              //used to load images
#include <cglm/cglm.h>              //used for maths

#include "shader.h"


//macros
#define degToRad(deg) ((deg) * M_PI / 180.0)
#define radToDeg(rad) ((rad) * 180.0 / M_PI)

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void glfw_error_callback(int code, const char *err_str);
GLFWwindow *initializeWindow();

float mixture = 0.0f;
int up_pressed = 0;
int down_pressed = 0;

int w_pressed = 0;
int a_pressed = 0;
int s_pressed = 0;
int d_pressed = 0;

float delta_time = 0.0f;
float last_frame = 0.0f;

float last_mouse_x = 400;
float last_mouse_y = 300;
int first_mouse = 1;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

vec3 cam_pos   = {0.0f, 0.0f,  3.0f};
vec3 cam_front = {0.0f, 0.0f, -1.0f};
vec3 cam_up    = {0.0f, 1.0f,  0.0f};

int main() {
    //initialize window
    GLFWwindow *window = initializeWindow();
    //load the opengl library
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initiate GLAD\n");
        return -1;
    }
    //enable depth test
    glEnable(GL_DEPTH_TEST);
    //initialize the shaders
    struct shader shader_program;
    if(!initializeShader(&shader_program, "vertexshader.glsl", "fragmentshader.glsl")) {
        printf("Error initializing shader_program\n");
        return -1;
    }


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
        //positions           //texture coordinates
         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    vec3 cubes[] = {
        { 0.0f,  0.0f,  0.0f},
        { 2.0f,  5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        { 2.4f, -0.4f, -3.5f},
        {-1.7f,  3.0f, -7.5f},
        { 1.3f, -2.0f, -2.5f},
        { 1.5f,  2.0f, -2.5f},
        { 1.5f,  0.2f, -1.5f},
        {-1.3f,  1.0f, -1.5f},
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //texture attributes
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shader_program.id);
    glUniform1i(glGetUniformLocation(shader_program.id, "texture0"), 0);
    glUniform1i(glGetUniformLocation(shader_program.id, "texture1"), 1);

    //Main loop
    while(!glfwWindowShouldClose(window)) {
        //process inputs
        processInput(window);

        //rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //bind textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        //use shader program
        glUseProgram(shader_program.id);
        glUniform1f(glGetUniformLocation(shader_program.id, "mixture"), mixture);
        glBindVertexArray(VAO);

        //constructs a lookat view matrix
        mat4 view;
        vec3 cam_dir;
        glm_vec_add(cam_pos, cam_front, cam_dir);
        glm_lookat(cam_pos, cam_dir, cam_up, view);
        glUniformMatrix4fv(glGetUniformLocation(shader_program.id, "view"), 1, GL_FALSE, (GLfloat *)view);

        //constructs the projection matrix
        mat4 projection;
        glm_perspective(degToRad(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f, projection);
        glUniformMatrix4fv(glGetUniformLocation(shader_program.id, "projection"), 1, GL_FALSE, (GLfloat *)projection);

        for(int i = 0; i < 10; i ++) {
            mat4 model;
            glm_translate_make(model, cubes[i]);
            glm_rotate(model, i * degToRad(20.0f) + (float)glfwGetTime() * degToRad(50.0f), (vec3){0.5f, 1.0f, 0.0f});

            glUniformMatrix4fv(glGetUniformLocation(shader_program.id, "model"), 1, GL_FALSE, (GLfloat *)model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //unbind vertex array
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
    int w = glfwGetKey(window, GLFW_KEY_W);
    int a = glfwGetKey(window, GLFW_KEY_A);
    int s = glfwGetKey(window, GLFW_KEY_S);
    int d = glfwGetKey(window, GLFW_KEY_D);

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

    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    float cam_speed = 2.5f * delta_time;
    vec3 temp;
    if(w == GLFW_PRESS) {
        glm_vec_muladds(cam_front, cam_speed, cam_pos);
    }
    if(a == GLFW_PRESS) {
        glm_vec_cross(cam_front, cam_up, temp);
        glm_normalize(temp);
        glm_vec_muladds(temp, -1 * cam_speed, cam_pos);
    }
    if(s == GLFW_PRESS) {
        glm_vec_muladds(cam_front, -1 * cam_speed, cam_pos);
    }
    if(d == GLFW_PRESS) {
        glm_vec_cross(cam_front, cam_up, temp);
        glm_normalize(temp);
        glm_vec_muladds(temp, cam_speed, cam_pos);
    }

}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
    //prevent sudden camera jump when mouse enters window
    if(first_mouse) {
        last_mouse_x = x_pos;
        last_mouse_y = y_pos;
        first_mouse = 0;
    }

    float x_offset = x_pos - last_mouse_x;
    float y_offset = last_mouse_y - y_pos;
    last_mouse_x = x_pos;
    last_mouse_y = y_pos;

    float sensitivity = 0.05f;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    cam_front[0] = cos(degToRad(yaw)) * cos(degToRad(pitch));
    cam_front[1] = sin(degToRad(pitch));
    cam_front[2] = sin(degToRad(yaw)) * cos(degToRad(pitch));
    glm_normalize(cam_front);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    if(fov >= 1.0f && fov <= 45.0f)
        fov -= y_offset;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;
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

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "TITLE", NULL, NULL);

    if(window == NULL) {
        printf("Error creating window\n");
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    return window;
}
