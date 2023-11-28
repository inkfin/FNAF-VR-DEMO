#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

#include <Game/GlobalObjects.h>
#include <Game/CameraInterface.h>

/**
 * An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
 * Handle the movement logic and part of character logic for now
 */
class Camera : public ICameraInterface {
public:
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Default camera values
    static constexpr float YAW = -90.0f;
    static constexpr float PITCH = 0.0f;
    static constexpr float SPEED = 2.5f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM = 45.0f;

    bool move_forward_flag = false;
    bool move_back_flag = false;
    bool move_left_flag = false;
    bool move_right_flag = false;
    bool move_up_flag = false;
    bool move_down_flag = false;

    // camera Attributes
    glm::vec3 Position { 0.0f, 0.0f, 1.0f };
    glm::vec3 Front { 0.0f, 0.0f, -1.0f };
    glm::vec3 Up { 0.0f, 1.0f, 0.0f };
    glm::vec3 Right {};
    glm::vec3 WorldUp { 0.0f, 1.0f, 0.0f };
    // euler Angles
    float Yaw = YAW;
    float Pitch = PITCH;
    // camera options
    float MovementSpeed = SPEED;
    float MouseSensitivity = SENSITIVITY;
    float Zoom = ZOOM;

    Camera()
    {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    [[nodiscard]] glm::mat4 GetViewMatrix() const override
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    [[nodiscard]] glm::vec3 GetForward() const override
    {
        return Front;
    }

    [[nodiscard]] glm::vec3 GetUp() const override
    {
        return Up;
    }

    [[nodiscard]] glm::vec3 GetRight() const override
    {
        return Right;
    }

    [[nodiscard]] glm::vec3 GetLocation() const override
    {
        return Position;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, double deltaTime)
    {
        float velocity = MovementSpeed * (float)deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void Move(double deltaTime) override
    {
        // handle movements
        if (move_forward_flag && !move_back_flag) {
            ProcessKeyboard(FORWARD, deltaTime);
        }
        if (move_back_flag && !move_forward_flag) {
            ProcessKeyboard(BACKWARD, deltaTime);
        }
        if (move_right_flag && !move_left_flag) {
            ProcessKeyboard(RIGHT, deltaTime);
        }
        if (move_left_flag && !move_right_flag) {
            ProcessKeyboard(LEFT, deltaTime);
        }
        if (move_up_flag && !move_down_flag) {
            ProcessKeyboard(UP, deltaTime);
        }
        if (move_down_flag && !move_up_flag) {
            ProcessKeyboard(DOWN, deltaTime);
        }

        // update shader eye position
        Scene::SceneData.eye_w = glm::vec4(Position, 1.0f);
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};