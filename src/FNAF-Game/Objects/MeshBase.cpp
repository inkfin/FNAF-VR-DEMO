#include "MeshBase.h"
#include <GL/glew.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void MeshBase::CalcMeshBoundingBox(const aiMesh* mesh, glm::vec3& min, glm::vec3& max)
{
    min.x = min.y = min.z = 1e10f;
    max.x = max.y = max.z = -1e10f;

    for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {
        aiVector3D tmp = mesh->mVertices[t];

        min.x = std::min(min.x, tmp.x);
        min.y = std::min(min.y, tmp.y);
        min.z = std::min(min.z, tmp.z);

        max.x = std::max(max.x, tmp.x);
        max.y = std::max(max.y, tmp.y);
        max.z = std::max(max.z, tmp.z);
    }
}

void MeshBase::CalcNodeBoundingBox(const aiNode* nd, glm::vec3& min, glm::vec3& max)
{
    for (unsigned int n = 0; n < nd->mNumMeshes; ++n) {
        const aiMesh* mesh = mScene->mMeshes[nd->mMeshes[n]];
        for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {

            aiVector3D tmp = mesh->mVertices[t];

            min.x = std::min(min.x, tmp.x);
            min.y = std::min(min.y, tmp.y);
            min.z = std::min(min.z, tmp.z);

            max.x = std::max(max.x, tmp.x);
            max.y = std::max(max.y, tmp.y);
            max.z = std::max(max.z, tmp.z);
        }
    }

    for (unsigned int n = 0; n < nd->mNumChildren; ++n) {
        CalcNodeBoundingBox(nd->mChildren[n], min, max);
    }
}

bool MeshBase::ValidMeshFilename(const std::string& absolutePath)
{
    // check if file exists
    std::ifstream fin(absolutePath);
    if (fin.fail()) {
        printf("Couldn't open file: %s\n", absolutePath.c_str());
        return false;
    } else {
        fin.close();
    }

    static Assimp::Importer gImporter;
    std::string ext = fs::path(absolutePath).extension().string();
    return gImporter.IsExtensionSupported(ext);
}

glm::mat4 MeshBase::GetModelMatrix() const
{
    // Apply TRS
    glm::mat4 result;
    result = glm::scale(mScale);

    result = glm::rotate(glm::radians(mRotation.x), glm::vec3(1.f, 0.f, 0.f)) * result;
    result = glm::rotate(glm::radians(mRotation.y), glm::vec3(0.f, 1.f, 0.f)) * result;
    result = glm::rotate(glm::radians(mRotation.z), glm::vec3(0.f, 0.f, 1.f)) * result;

    result = glm::translate(mTranslation) * result;
    return result;
}
