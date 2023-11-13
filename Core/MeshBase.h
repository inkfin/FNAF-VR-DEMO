#ifndef __MESHASSET_H__
#define __MESHASSET_H__

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

namespace UniformLoc {
const int PV = 0;
const int M = 1;
const int Time = 2;
const int NumBones = 3;
const int Mode = 4;
const int DebugID = 5;
const int Bones = 20; // array of 100 bones
};

namespace AttribLoc {
const int Pos = 0;
const int TexCoord = 1;
const int Normal = 2;
const int BoneIds = 3;
const int BoneWeights = 4;
};

struct SubmeshData {
    unsigned int mNumIndices;
    unsigned int mBaseIndex;
    unsigned int mBaseVertex;

    SubmeshData()
        : mNumIndices(0)
        , mBaseIndex(0)
        , mBaseVertex(0)
    {
    }
    void DrawSubmesh() const;
};

class MeshBase {
protected:
    Assimp::Importer mImporter;

public:
    const aiScene* mScene = nullptr;

    unsigned int mVao = -1;
    unsigned int mVboVerts = -1;
    unsigned int mVboNormals = -1;
    unsigned int mVboTexCoords = -1;
    unsigned int mIndexBuffer = -1;
    glm::vec3 mScaleFactor = glm::vec3(1.0f);

    glm::vec3 mBbMin = glm::vec3(0.0f);
    glm::vec3 mBbMax = glm::vec3(0.0f);

    std::vector<SubmeshData> mSubmesh;

    MeshBase() {};

    MeshBase(const std::string& filename)
    {
        LoadMesh(filename);
    }

    MeshBase(const aiScene* scene)
        : mScene(scene)
    {
    }

    ~MeshBase() { Clear(); }

    virtual void Clear();

    virtual bool LoadMesh(const std::string& filename, // absolute path to mesh
        const bool normalScale = true, // scaled by its maximum bounding box
        const unsigned int flags = aiProcessPreset_TargetRealtime_Quality | aiProcess_PreTransformVertices | aiProcess_Triangulate // load flags
    );
    void DrawMesh() const;

    virtual void Render();

protected:
    void BufferIndexedVerts();
    void GetBoundingBox(const aiScene* scene, glm::vec3* min, glm::vec3* max);
    void GetBoundingBox(const aiMesh* mesh, glm::vec3* min, glm::vec3* max);
    void GetBoundingBoxForNode(const aiScene* scene, const aiNode* nd, glm::vec3* min, glm::vec3* max);

public:
    static bool ValidMeshFilename(const std::string& absolutePath);
};

#endif // !__MESHASSET_H__
