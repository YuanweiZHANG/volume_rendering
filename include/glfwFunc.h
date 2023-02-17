#ifndef GLFWFUNC_H
#define GLFWFUNC_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "camera.h"

#include <iostream>
#include <vector>
#include <cmath>


enum Axis {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};

void rotate(Axis axis, glm::mat4 &rotation, float angle) {
    switch (axis)
    {
    case X_AXIS:
        rotation = glm::mat4(
            1,          0,           0, 0,
            0, cos(angle), -sin(angle), 0,
            0, sin(angle),  cos(angle), 0,
            0,          0,           0, 1
        );
        break;
    case Y_AXIS:
        rotation = glm::mat4(
            cos(angle),  0, sin(angle), 0,
            0,           1,          0, 0,
           -sin(angle),  0, cos(angle), 0,
            0,           0,          0, 1
        );
        break;
    case Z_AXIS:
        rotation = glm::mat4(
            cos(angle), -sin(angle), 0, 0,
            sin(angle),  cos(angle), 0, 0,
            0,          0,           1, 0,
            0,          0,           0, 1
        );
        break;
    default:
        break;
    }
}

/**
 * @brief Rotate camera position so that the camera will always look at view center
 *        while rotating.
 * 
 * @param axis rotate along axis.
 * @param camera 
 * @param angle 
 * @param viewCenter the point which camera will always look at.
 */
void rotateCamera(Axis axis, Camera &camera, float angle, const glm::vec3 &viewCenter) {
    glm::mat4 trans1 = glm::mat4(1.0f);
    trans1 = glm::translate(trans1, -viewCenter);
    camera.Position = glm::vec3((trans1 * glm::vec4(camera.Position, 1.0f)));
    
    glm::vec3 a;
    switch (axis) {
    case X_AXIS:
        a = glm::vec3(1.0, 0.0, 0.0);
        break;
    case Y_AXIS:
        a = glm::vec3(0.0, 1.0, 0.0);
        break;
    case Z_AXIS:
        a = glm::vec3(0.0, 0.0, 1.0);
        break;
    default:
        a = glm::vec3(0.0);
        break;
    }
    glm::mat4 rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, angle, a);
    camera.Position = glm::vec3((rot * glm::vec4(camera.Position, 1.0f)));

    glm::mat4 trans2 = glm::mat4(1.0f);
    trans2 = glm::translate(trans2, viewCenter);
    camera.Position = glm::vec3((trans2 * glm::vec4(camera.Position, 1.0f)));
    camera.Front = viewCenter - camera.Position;
}

#endif
