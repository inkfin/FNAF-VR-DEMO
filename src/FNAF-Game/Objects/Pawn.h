//
// Created by boile on 2023/11/15.
//

#pragma once

#include <string>
#include <memory>
#include <glm/gtx/transform.hpp>
#include "MeshBase.h"
#include "StaticMesh.h"
#include "SkinnedMesh.h"
#include "AIComponent.h"

#define SHOCK_LEVEL_MIN 1
#define SHOCK_LEVEL_MAX 20

class Pawn {
public:
    int pawn_id;

    struct Status {
        int shock_level; // when shock_level is 0, the pawn is gone
        bool active;
        bool moving;
        float speed;
    } mStatus;

    glm::vec3 mTranslation = glm::vec3(0.f);
    glm::vec3 mRotation = glm::vec3(0.f);
    glm::vec3 mScale = glm::vec3(1.f);

    Pawn();

    ~Pawn();

    void Init();
    void Move(const float delta_time, const glm::vec3& direction);


    std::unique_ptr<MeshBase> mMesh;
    AIComponent mAIComponent;

    glm::mat4 GetModelMatrix() const;
};
