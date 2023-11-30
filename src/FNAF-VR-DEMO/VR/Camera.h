//
// Created by 11096 on 11/19/2023.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Game/GlobalObjects.h"
#include "Game/CameraInterface.h"

class Camera : public ICameraInterface {

private:
    glm::mat4 mViewMatrixRaw { glm::identity<glm::mat4>() };

    glm::vec3 mForward { 0.f, 0.f, -1.f };
    glm::vec3 mRight { 1.f, 0.f, 0.f };
    glm::vec3 mUp { 0.f, 1.f, 0.f };

    glm::vec3 mLocation { 0.f, 0.f, 0.f };
    const glm::vec3 loc_offset { 0.0f, 0.0f, 1.0f };
    glm::mat4 mRotation { glm::identity<glm::mat4>() };

public:
    float speed = 100.0;

    [[nodiscard]] glm::mat4 GetViewMatrix() const override
    {
        glm::mat4 retView = mViewMatrixRaw * glm::translate(-mLocation);
            return retView;
    }

    [[nodiscard]] glm::vec3 GetForward() const override
    {
        return mForward;
    }

    [[nodiscard]] glm::vec3 GetUp() const override
    {
        return mUp;
    }

    [[nodiscard]] glm::vec3 GetRight() const override
    {
        return mRight;
    }

    [[nodiscard]] glm::vec3 GetLocation() const override
    {
        return mLocation + loc_offset;
    }

    static glm::mat4 RemoveScaleFromMatrix(const glm::mat4& matrix)
    {
        glm::mat4 result = matrix;

        for (int i = 0; i < 3; ++i) {
            float length = glm::length(glm::vec3(result[i]));
            assert(length > 1e-8);
            result[i] /= length;
        }

        return result;
    }

    // get camera rotation in world space
    [[nodiscard]] glm::mat4 GetRotation() const
    {
        return mRotation;
    }

    void Update(const glm::mat4& V)
    {
        mViewMatrixRaw = V;

        // update orientation
        mForward = -glm::normalize(glm::vec3(V[0][2], V[1][2], V[2][2]));
        mUp = glm::normalize(glm::vec3(V[0][1], V[1][1], V[2][1]));
        mRight = glm::normalize(glm::vec3(V[0][0], V[1][0], V[2][0]));

        glm::mat4 inV = glm::inverse(V);
        inV = RemoveScaleFromMatrix(inV);
        inV[3][0] = inV[3][1] = inV[3][2] = 0.f;
        mRotation = inV;
    }

    // update view matrix
    void Move(double deltaTime) override
    {
        const glm::vec2& p = Scene::gControllerState.trackpad_left;
        // get the correct direction input
        glm::vec4 delta_pos(p.x, 0.f, -p.y, 0.f);

        // get current camera orientation
        const glm::mat4 rotation = GetRotation();

        // get the correct direction in world space
        delta_pos = rotation * delta_pos;

        delta_pos = delta_pos * speed * (float)deltaTime;
        mLocation += glm::vec3(delta_pos.x, delta_pos.y, delta_pos.z);

        // update shader eye position
        Scene::SceneData.eye_w = glm::vec4(GetLocation(), 1.0f);
    }
};
