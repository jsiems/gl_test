
#include <stdio.h>
#include <math.h>

#include <glad/glad.h>              //defines opengl functions, etc
#include <GLFW/glfw3.h>             //used for window and input
#include <cglm/cglm.h>              //used for maths

#define STB_IMAGE_IMPLEMENTATION

#include "shader.h"
#include "camera.h"
#include "model.h"

//macros
#define degToRad(deg) ((deg) * M_PI / 180.0)
#define radToDeg(rad) ((rad) * 180.0 / M_PI)

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FPS_LIMIT 60.0f

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, struct Camera *cam);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void glfw_error_callback(int code, const char *err_str);
GLFWwindow *initializeWindow();
unsigned int loadTexture(char * name);


float delta_time = 0.0f;
float last_frame = 0.0f;

float last_mouse_x = 400;
float last_mouse_y = 300;
uint8_t first_mouse = 1;

uint8_t flashlight_on = 0;

uint8_t draw_wireframe = 0;
uint8_t t_pressed = 0;

struct Camera cam;

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
    
    //initialize the camera
    initializeCamera(&cam, (vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f, 2.5f, 0.1f, 45.0f);


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

    vec3 point_lights[] = {
        { 0.7f,  0.2f,  2.0f},
        { 2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        { 0.0f,  0.0f, -3.0f}
    };

    struct Model cube;
    initializeModel(&cube, "mushroom");

    struct Model light;
    initializeModel(&light, "lightbulb");

    struct Model crate;
    initializeModel(&crate, "crate");

    //keep track of FPS
    uint64_t total_frames = 0;
    float start_time = glfwGetTime();
    float min_frame_time = 1 / FPS_LIMIT;

    //Main loop
    while(!glfwWindowShouldClose(window)) {
        //wait for max FPS limit
        while(glfwGetTime() - last_frame < min_frame_time);

        //update time since last frame
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        total_frames ++;

        //process inputs
        processInput(window, &cam);

        //rendering commands here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //construct matrices for camera
        mat4 view, projection;
        getViewMatrix(&cam, view);
        glm_perspective(degToRad(cam.zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f, projection);

        drawModel(&light, 4, point_lights, 4, point_lights,
                  &view, &projection, &cam, flashlight_on);

        drawModel(&cube, 10, cubes, 4, point_lights,
                  &view, &projection, &cam, flashlight_on);

        vec3 cpos = {1.0f, 1.0f, -1.0f};
        drawModel(&crate, 1, &cpos, 4, point_lights, 
                  &view, &projection, &cam, flashlight_on);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyModel(&cube);
    destroyModel(&light);
    destroyModel(&crate);

    printf("End of program\n\tframes: %I64d\n\tTime: %f\n\tFPS: %f", total_frames, glfwGetTime() - start_time, total_frames / (glfwGetTime() - start_time));

    glfwTerminate();
    return 0;
}

//should definitely use key callback for this
//  keycallback insures that will we handle the input
//  even if they release the key before we process the input
//  in the loop
void processInput(GLFWwindow *window, struct Camera *cam) {
    int escape = glfwGetKey(window, GLFW_KEY_ESCAPE);
    int w = glfwGetKey(window, GLFW_KEY_W);
    int a = glfwGetKey(window, GLFW_KEY_A);
    int s = glfwGetKey(window, GLFW_KEY_S);
    int d = glfwGetKey(window, GLFW_KEY_D);
    int space = glfwGetKey(window, GLFW_KEY_SPACE);
    int ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);
    int shift = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
    int f = glfwGetKey(window, GLFW_KEY_F);
    int t = glfwGetKey(window, GLFW_KEY_T);

    //quit when escape is pressed
    if(escape == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1U);

    if(w == GLFW_PRESS) {
        translateCamera(cam, cam_forward, delta_time);
    }
    if(a == GLFW_PRESS) {
        translateCamera(cam, cam_left, delta_time);
    }
    if(s == GLFW_PRESS) {
        translateCamera(cam, cam_backward, delta_time);
    }
    if(d == GLFW_PRESS) {
        translateCamera(cam, cam_right, delta_time);
    }
    if(space == GLFW_PRESS) {
        translateCamera(cam, cam_up, delta_time);
    }
    if(ctrl == GLFW_PRESS) {
        translateCamera(cam, cam_down, delta_time);
    }
    if(shift == GLFW_PRESS) {
        boostCamera(cam, 1);
    }
    if(shift == GLFW_RELEASE) {
        boostCamera(cam, 0);
    }

    if(f == GLFW_PRESS) {
        flashlight_on = 1;
    }
    else if(f == GLFW_RELEASE) {
        flashlight_on = 0;
    }

    if(t == GLFW_PRESS && !t_pressed) {
        t_pressed = 1;
        draw_wireframe = ~draw_wireframe;
        if(draw_wireframe) {
            // draw wireframes
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    else if(t == GLFW_RELEASE) {
        t_pressed = 0;
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

    rotateCamera(&cam, x_offset, y_offset, 1U);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
    zoomCamera(&cam, y_offset);
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
