
#include "camera.h"

 void initializeCamera(struct camera *cam, vec3 position, vec3 up, float yaw, float pitch, float movement_speed, float mouse_sensitivity, float zoom) {
    glm_vec_copy(position, cam->position);
    glm_vec_copy(up, cam->world_up);
    cam->yaw = yaw;
    cam->pitch = pitch;
    cam->movement_speed = movement_speed;
    cam->mouse_sensitivity = mouse_sensitivity;
    cam->zoom = zoom;

    glm_vec_copy((vec3){0.0f, 0.0f, -1.0f}, cam->front);

    updateCameraVectors(cam);
}

void getViewMatrix(struct camera *cam, mat4 view) {
    vec3 cam_dir;
    glm_vec_add(cam->position, cam->front, cam_dir);
    glm_lookat(cam->position, cam_dir, cam->up, view);
}

void translateCamera(struct camera *cam, enum camera_movement direction, float delta_time) {
    float velocity = (cam->movement_speed + 2.0f * cam->boost * cam->movement_speed) * delta_time;
    if(direction == cam_forward) {
        glm_vec_muladds(cam->front, velocity, cam->position);
    }

    if(direction == cam_backward) {
        glm_vec_muladds(cam->front, -1 * velocity, cam->position);
    }

    if(direction == cam_left) {
        glm_vec_muladds(cam->right, -1 * velocity, cam->position);
    } 

    if(direction == cam_right) {
        glm_vec_muladds(cam->right, velocity, cam->position);
    }
}

void rotateCamera(struct camera *cam, float x_offset, float y_offset, uint8_t constrain_pitch) {
    x_offset *= cam->mouse_sensitivity;
    y_offset *= cam->mouse_sensitivity;

    cam->yaw = fmod(cam->yaw + x_offset, 360.0f);
    cam->pitch += y_offset;

    //move magic numbers to constants
    if(constrain_pitch) {
        if(cam->pitch > 89.0f)
            cam->pitch = 89.0f;
        if(cam->pitch < -89.0f)
            cam->pitch = -89.0f;
    }

    updateCameraVectors(cam);
}

void zoomCamera(struct camera *cam, float y_offset) {
    //can move these constraints to constants
    if(cam->zoom >= 1.0f && cam->zoom <= 45.0f)
        cam->zoom -= y_offset;
    if(cam->zoom <= 1.0f)
        cam->zoom = 1.0f;
    if(cam->zoom >= 45.0f)
        cam->zoom = 45.0f;
}

void updateCameraVectors(struct camera *cam) {
    vec3 temp;
    cam->front[0] = cos(degToRad(cam->yaw)) * cos(degToRad(cam->pitch));
    cam->front[1] = sin(degToRad(cam->pitch));
    cam->front[2] = sin(degToRad(cam->yaw)) * cos(degToRad(cam->pitch));
    glm_normalize(cam->front);

    glm_vec_cross(cam->front, cam->world_up, temp);
    glm_normalize_to(temp, cam->right);
    glm_vec_cross(cam->right, cam->front, temp);
    glm_normalize_to(temp, cam->up);
}

void boostCamera(struct camera *cam, int boost) {
    cam->boost = boost;
}
