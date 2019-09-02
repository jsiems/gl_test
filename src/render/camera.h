#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>

//consider moving these to a global constants or macros header file
#define degToRad(deg) ((deg) * M_PI / 180.0)
#define radToDeg(rad) ((rad) * 180.0 / M_PI)

struct Camera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;

    float yaw;
    float pitch;

    int boost;
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
};

enum camera_movement {
    cam_forward,
    cam_backward,
    cam_left,
    cam_right,
    cam_up,
    cam_down
};

void initializeCamera(struct Camera *cam, vec3 position, vec3 up, float yaw, float pitch, float movement_speed, float mouse_sensitivity, float zoom);

void getViewMatrix(struct Camera * cam, mat4 view);

void translateCamera(struct Camera *cam, enum camera_movement direction, float delta_time);

void rotateCamera(struct Camera *cam, float x_offset, float y_offset, uint8_t constrain_pitch);

void zoomCamera(struct Camera *cam, float y_offset);

void updateCameraVectors(struct Camera *cam);

void boostCamera(struct Camera *cam, int boost);

#endif