//
// Created by 11096 on 11/19/2023.
//

#pragma once

class ICameraInterface {
public:
    // Move the camera based on user input
    virtual void Move(double deltaTime) = 0;
    virtual ~ICameraInterface() = default;

    virtual glm::mat4 GetViewMatrix() const = 0;

    virtual glm::vec3 GetForward() const = 0;
    virtual glm::vec3 GetUp() const = 0;
    virtual glm::vec3 GetRight() const = 0;

    virtual glm::vec3 GetLocation() const = 0;
};
