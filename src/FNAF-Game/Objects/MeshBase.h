#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LoadMesh.h"

class MeshBase {
protected:
    Assimp::Importer mImporter;

public:
    const aiScene* mScene = nullptr;

    glm::vec3 mBbMin = glm::vec3(0.0f);
    glm::vec3 mBbMax = glm::vec3(0.0f);

    glm::vec3 mTranslation = glm::vec3(0.0f);
    // Euler angle, apply by x, y, z
    glm::vec3 mRotation = glm::vec3(0.0f);
    glm::vec3 mScale = glm::vec3(1.0f);

    std::vector<SubMeshData> mSubMesh;

    MeshBase() = default;

    virtual bool LoadMesh(const std::string& filename) = 0;

    [[nodiscard]] glm::mat4 GetModelMatrix() const;

    virtual void Render() = 0;
    virtual void Update(float deltaTime) = 0;

protected:
    static void CalcMeshBoundingBox(const aiMesh* mesh, glm::vec3& min, glm::vec3& max);
    void CalcNodeBoundingBox(const aiNode* nd, glm::vec3& min, glm::vec3& max);

public:
    static bool ValidMeshFilename(const std::string& absolutePath);
};
