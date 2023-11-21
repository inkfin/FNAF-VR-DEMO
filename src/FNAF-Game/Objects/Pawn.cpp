//
// Created by boile on 2023/11/15.
//

#include "Pawn.h"

Pawn::Pawn()
{
    Init();
}

Pawn::~Pawn()
{
    Init();
}

void Pawn::Init()
{
    mStatus.shock_level = SHOCK_LEVEL_MAX;
    mStatus.active = true;
    mStatus.moving = true;
    mStatus.speed = 10.f;
}

void Pawn::Move(const float delta_time, const glm::vec3& direction)
{
    mTranslation += (delta_time * mStatus.speed * direction);
}

glm::mat4 Pawn::GetModelMatrix() const
{
    // Apply TRS
    glm::mat4 result;
    result = mMesh->GetModelMatrix();
    result = glm::scale(mScale) * result;

    result = glm::rotate(glm::radians(mRotation.x), glm::vec3(1.f, 0.f, 0.f)) * result;
    result = glm::rotate(glm::radians(mRotation.y), glm::vec3(0.f, 1.f, 0.f)) * result;
    result = glm::rotate(glm::radians(mRotation.z), glm::vec3(0.f, 0.f, 1.f)) * result;

    glm::mat4 test_trans = glm::translate(mTranslation);
    result = glm::translate(mTranslation) * result;

    return result;
}
