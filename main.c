
#include <stdio.h>
#include <math.h>
#include <glad/glad.h>              //defines opengl functions, etc
#include <GLFW/glfw3.h>             //used for window and input
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>              //used to load images
#include <cglm/cglm.h>              //used for maths

#include "shader.h"
#include "camera.h"


//macros
#define degToRad(deg) ((deg) * M_PI / 180.0)
#define radToDeg(rad) ((rad) * 180.0 / M_PI)

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FPS_LIMIT 60.0f

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, struct camera *cam);
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

struct camera cam;
uint8_t draw_wireframe = 0;
uint8_t t_pressed = 0;

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
    struct shader object_shader, light_shader;
    if(!initializeShader(&object_shader, "shaders/object_vs.glsl", "shaders/object_fs.glsl")) {
        printf("Error initializing object shaders\n");
        return -1;
    }
    if(!initializeShader(&light_shader, "shaders/light_vs.glsl", "shaders/light_fs.glsl")) {
        printf("Error initializing light shader\n");
        return -1;
    }

    
    //initialize the camera
    initializeCamera(&cam, (vec3){0.0f, 0.0f, 3.0f}, (vec3){0.0f, 1.0f, 0.0f}, -90.0f, 0.0f, 2.5f, 0.1f, 45.0f);


    //initialize textures
    unsigned int crate_texture, crate_spec_map;
    crate_texture = loadTexture("container2.png");
    crate_spec_map = loadTexture("container2_specular.png");


    //-------------vertex data and buffers

    //positions and normal vectors AND texture data
    float vertices[] = {
        //positions           //normals            //textures 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
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

    vec3 point_lights[] = {
        { 0.7f,  0.2f,  2.0f},
        { 2.3f, -3.3f, -4.0f},
        {-4.0f,  2.0f, -12.0f},
        { 0.0f,  0.0f, -3.0f}
    };

    //cube Vertex array object n such
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //bind the vertex array object
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //lamp vertex array object
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //we don't need to buffer data for VBO because it has alreayd been done
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //tell the shader which texture to use for which uniform
    glUseProgram(object_shader.id);
    setInt(&object_shader, "material.diffuse", 0);
    setInt(&object_shader, "material.specular", 1);


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

        // ********** LIGHT UNIFORMS && DRAWING **************
        //Configure uniforms and draw vertices for light
        glUseProgram(light_shader.id);
        glBindVertexArray(lightVAO);
        setMat4(&light_shader, "view", view);
        setMat4(&light_shader, "projection", projection);
        for(int i = 0; i < 4; i ++) {
            mat4 model;
            glm_translate_make(model, point_lights[i]);
            glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
            setMat4(&light_shader, "model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // ********** OBJECTS UNIFORMS && DRAWING **********
        //Configure uniforms and draw vertices for cubes
        glUseProgram(object_shader.id);
        glBindVertexArray(VAO);
        setMat4(&object_shader, "view", view);
        setMat4(&object_shader, "projection", projection);

        //sets the objects color and the color of the light hitting the object
        float shininess = 32.0f;
        setFloat(&object_shader, "material.shininess", shininess);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, crate_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, crate_spec_map);

        //light colors n such
        vec3 light_color = {1.0f, 1.0f, 1.0f};
        vec3 diffuse_color;
        glm_vec_mul(light_color, (vec3){0.5f, 0.5f, 0.5f}, diffuse_color);
        vec3 ambient_color;
        glm_vec_mul(diffuse_color, (vec3){0.2f, 0.2f, 0.2f}, ambient_color);
        setVec3(&object_shader, "dir_light.direction", (vec3){-0.2f, -1.0f, -0.3f});
        setVec3(&object_shader, "dir_light.ambient", ambient_color);
        setVec3(&object_shader, "dir_light.diffuse", diffuse_color);
        setVec3(&object_shader, "dir_light.specular", (vec3){1.0f, 1.0f, 1.0f});

        //set uniforms for point lights
        
        float light_const = 1.0f, light_lin = 0.09f, light_quad = 0.032f;
        for(int i = 0; i < 4; i ++) {
            char uniform[] = "point_lights[i].";
            uniform[13] = i + '0';
            char uniform_str[30] = "";

            //uniform_str = "point_lights[i].position"
            strcpy(uniform_str, uniform); strcat(uniform_str, "position");
            setVec3(&object_shader, uniform_str, point_lights[i]);

            strcpy(uniform_str, uniform); strcat(uniform_str, "ambient");
            setVec3(&object_shader, uniform_str, ambient_color);

            strcpy(uniform_str, uniform); strcat(uniform_str, "diffuse");
            setVec3(&object_shader, uniform_str, diffuse_color);

            strcpy(uniform_str, uniform); strcat(uniform_str, "specular");
            setVec3(&object_shader, uniform_str, (vec3){1.0f, 1.0f, 1.0f});

            strcpy(uniform_str, uniform); strcat(uniform_str, "constant");
            setFloat(&object_shader, uniform_str, light_const);

            strcpy(uniform_str, uniform); strcat(uniform_str, "linear");
            setFloat(&object_shader, uniform_str, light_lin);

            strcpy(uniform_str, uniform); strcat(uniform_str, "quadratic");
            setFloat(&object_shader, uniform_str, light_quad);
        }

        //uniforms for spotlight
        float cutoff = cos(degToRad(12.5f));
        float outer_cutoff = cos(degToRad(17.f));
        setVec3(&object_shader, "spot_light.position", cam.position);
        setVec3(&object_shader, "spot_light.direction", cam.front);
        setFloat(&object_shader, "spot_light.constant", light_const);
        setFloat(&object_shader, "spot_light.linear", light_lin);
        setFloat(&object_shader, "spot_light.quadratic", light_quad);
        setVec3(&object_shader, "spot_light.diffuse", diffuse_color);
        setVec3(&object_shader, "spot_light.specular", (vec3){1.0f, 1.0f, 1.0f});
        setFloat(&object_shader, "spot_light.cutoff", cutoff);
        setFloat(&object_shader, "spot_light.outer_cutoff", outer_cutoff);
        setInt(&object_shader, "spot_light.on", flashlight_on);
        setVec3(&object_shader, "view_pos", cam.position);

        for(int i = 0; i < 10; i ++) {
            mat4 model;
            glm_translate_make(model, cubes[i]);
            glm_rotate(model, i * degToRad(20.0f), (vec3){0.5f, 1.0f, 0.0f});
            setMat4(&object_shader, "model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //unbind vertex array
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    printf("End of program\n\tframes: %I64d\n\tTime: %f\n\tFPS: %f", total_frames, glfwGetTime() - start_time, total_frames / (glfwGetTime() - start_time));

    glfwTerminate();
    return 0;
}

//should definitely use key callback for this
//  keycallback insures that will we handle the input
//  even if they release the key before we process the input
//  in the loop
void processInput(GLFWwindow *window, struct camera *cam) {
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

unsigned int loadTexture(char * name) {
    unsigned char *image_data;
    int image_width, image_height, nr_channels;
    unsigned int texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    image_data = stbi_load(name, &image_width, &image_height, &nr_channels, 0);
    if(image_data == NULL) {
        printf("Failed to load texture %s\n", name);
        return -1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    return texture;
}
