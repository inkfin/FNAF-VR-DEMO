#include "MeshBase.h"
#include <GL/glew.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

MeshBase::~MeshBase()
{
    if (mScene != nullptr) {
        mImporter.FreeScene();
        mScene = nullptr;
    }
}

void MeshBase::Clear()
{
    mScaleFactor = glm::vec3(1.0f);

    if (mScene != nullptr) {
        mImporter.FreeScene();
        mScene = nullptr;
    }

    if (mVao != -1) {
        glDeleteVertexArrays(1, &mVao);
    }

    if (mIndexBuffer != -1) {
        glDeleteBuffers(1, &mIndexBuffer);
    }

    if (mVboVerts != -1) {
        glDeleteBuffers(1, &mVboVerts);
    }

    if (mVboTexCoords != -1) {
        glDeleteBuffers(1, &mVboTexCoords);
    }

    if (mVboNormals != -1) {
        glDeleteBuffers(1, &mVboNormals);
    }
}

//bool MeshBase::LoadMesh(const std::string& filename)
//{
//    return LoadMesh(filename, true,
//        aiProcessPreset_TargetRealtime_Quality | aiProcess_PreTransformVertices | aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
//}

bool MeshBase::LoadMesh(const std::string& filename, const bool normalScale, const unsigned int flags)
{
    // check if file exists
    std::ifstream fin(filename);
    if (fin.fail()) {
        printf("Couldn't open file: %s\n", filename.c_str());
        return false;
    } else {
        fin.close();
    }

    // Reset all class members
    Clear();

    mScene = mImporter.ReadFile(filename, flags);

    if (mScene == nullptr) {
        printf("Error parsing '%s': '%s'\n", filename.c_str(), mImporter.GetErrorString());
        return false;
    }

    printf("Import of scene %s succeeded.\n", filename.c_str());

    GetBoundingBox(mScene, &mBbMin, &mBbMax);

    // scale its longgest edge to 1.0
    if (normalScale) {
        glm::vec3 diff = mBbMax - mBbMin;
        float w = glm::max(diff.x, glm::max(diff.y, diff.z));
        mScaleFactor = glm::vec3(1.0f / w);
    }

    // create GL buffer objects and copy data
    BufferIndexedVerts();

    return true;
}

void SubmeshData::DrawSubmesh() const
{
    // always draw triangles
    glDrawElementsBaseVertex(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mBaseIndex), mBaseVertex);
}

void MeshBase::DrawMesh() const
{
    for (int m = 0; m < mSubmesh.size(); m++) {
        mSubmesh[m].DrawSubmesh();
    }
}

//void MeshBase::Render()
//{
//    DrawMesh();
//}

void MeshBase::GetBoundingBox(const aiScene* scene, glm::vec3* min, glm::vec3* max)
{
    min->x = min->y = min->z = 1e10f;
    max->x = max->y = max->z = -1e10f;
    GetBoundingBoxForNode(scene, scene->mRootNode, &mBbMin, &mBbMax);
}

void MeshBase::GetBoundingBox(const aiMesh* mesh, glm::vec3* min, glm::vec3* max)
{
    min->x = min->y = min->z = 1e10f;
    max->x = max->y = max->z = -1e10f;

    for (unsigned int t = 0; t < mesh->mNumVertices; ++t) {
        aiVector3D tmp = mesh->mVertices[t];

        min->x = std::min(min->x, tmp.x);
        min->y = std::min(min->y, tmp.y);
        min->z = std::min(min->z, tmp.z);

        max->x = std::max(max->x, tmp.x);
        max->y = std::max(max->y, tmp.y);
        max->z = std::max(max->z, tmp.z);
    }
}

void MeshBase::GetBoundingBoxForNode(const aiScene* scene, const aiNode* nd, glm::vec3* min, glm::vec3* max)
{
    unsigned int n = 0, t;

    for (; n < nd->mNumMeshes; ++n) {
        const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumVertices; ++t) {

            aiVector3D tmp = mesh->mVertices[t];

            min->x = std::min(min->x, tmp.x);
            min->y = std::min(min->y, tmp.y);
            min->z = std::min(min->z, tmp.z);

            max->x = std::max(max->x, tmp.x);
            max->y = std::max(max->y, tmp.y);
            max->z = std::max(max->z, tmp.z);
        }
    }

    for (n = 0; n < nd->mNumChildren; ++n) {
        GetBoundingBoxForNode(scene, nd->mChildren[n], min, max);
    }
}

void MeshBase::BufferIndexedVerts()
{
    // Buffer objects had been cleared before this function is called

    // Get current shader program
    GLint program = -1;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    // shader attrib locations
    constexpr int pos_loc = 0;
    constexpr int tex_coord_loc = 1;
    constexpr int normal_loc = 2;

    glBindAttribLocation(program, pos_loc, "pos_attrib");
    glBindAttribLocation(program, tex_coord_loc, "tex_coord_attrib");
    glBindAttribLocation(program, normal_loc, "normal_attrib");

    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);

    // Get all submeshes from assimp importer
    const int numSubmeshes = mScene->mNumMeshes;
    mSubmesh.resize(numSubmeshes);

    int totalNumVerts = 0;
    int totalNumIndices = 0;

    for (int m = 0; m < numSubmeshes; m++) {
        mSubmesh[m].mNumIndices = mScene->mMeshes[m]->mNumFaces * 3;
        mSubmesh[m].mBaseIndex = totalNumIndices;
        mSubmesh[m].mBaseVertex = totalNumVerts;

        totalNumVerts += mScene->mMeshes[m]->mNumVertices;
        totalNumIndices += mSubmesh[m].mNumIndices;
    }

    // Export indices from assimp format
    std::vector<unsigned int> indices(totalNumIndices);

    unsigned int faceIndex = 0;
    for (int m = 0; m < numSubmeshes; m++) {
        const int meshFaces = mScene->mMeshes[m]->mNumFaces;
        for (unsigned int f = 0; f < meshFaces; ++f) {
            const aiFace* face = &mScene->mMeshes[m]->mFaces[f];
            const unsigned int faceSize = face->mNumIndices;

            assert(faceSize == 3); // only triangles are supported

            // "push_back" a face to the indices vector
            memcpy(&indices[faceIndex], face->mIndices, faceSize * sizeof(unsigned int));

            faceIndex += faceSize;
        }
    }

    // Buffer indices
    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * totalNumIndices, indices.data(), GL_STATIC_DRAW);

    // Buffer vertices
    {
        glGenBuffers(1, &mVboVerts);
        glBindBuffer(GL_ARRAY_BUFFER, mVboVerts);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * totalNumVerts, 0, GL_STATIC_DRAW);

        int offset = 0;
        for (int m = 0; m < numSubmeshes; m++) {
            aiMesh* mesh = mScene->mMeshes[m];
            if (mesh->HasPositions()) {
                // TODO for animated meshes: inline aiNode* FindNode(const aiString& name), and compute transformation
                // aiNode* node = FindNode(mesh->mName);

                const int size = 3 * sizeof(float) * mesh->mNumVertices;
                const void* data = mesh->mVertices;
                glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
                offset += size;
            }
        }
        glEnableVertexAttribArray(pos_loc);
        glVertexAttribPointer(pos_loc, 3, GL_FLOAT, 0, 0, 0);
    }

    // buffer texture coordinates
    {
        glGenBuffers(1, &mVboTexCoords);
        glBindBuffer(GL_ARRAY_BUFFER, mVboTexCoords);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * totalNumVerts, 0, GL_STATIC_DRAW);

        int offset = 0;
        for (int m = 0; m < numSubmeshes; m++) {
            aiMesh* mesh = mScene->mMeshes[m];
            std::vector<float> tex_coords(2 * mesh->mNumVertices);
            if (mesh->HasTextureCoords(0)) {
                for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
                    tex_coords[k * 2] = mesh->mTextureCoords[0][k].x;
                    tex_coords[k * 2 + 1] = mesh->mTextureCoords[0][k].y;
                }
                const int size = 2 * sizeof(float) * mesh->mNumVertices;
                const void* data = tex_coords.data();
                glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
                offset += size;
            }
        }
        glEnableVertexAttribArray(tex_coord_loc);
        glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, 0, 0, 0);
    }

    // buffer normals
    {
        glGenBuffers(1, &mVboNormals);
        glBindBuffer(GL_ARRAY_BUFFER, mVboNormals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * totalNumVerts, 0, GL_STATIC_DRAW);

        int offset = 0;
        for (int m = 0; m < numSubmeshes; m++) {
            aiMesh* mesh = mScene->mMeshes[m];
            std::vector<float> normals(3 * mesh->mNumVertices);
            if (mesh->HasNormals()) {
                for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {
                    normals[k * 3] = mesh->mNormals[k].x;
                    normals[k * 3 + 1] = mesh->mNormals[k].y;
                    normals[k * 3 + 2] = mesh->mNormals[k].z;
                }
            }
            const int size = 3 * sizeof(float) * mesh->mNumVertices;
            const void* data = normals.data();
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
            offset += size;
        }
        glEnableVertexAttribArray(normal_loc);
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, 0, 0, 0);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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