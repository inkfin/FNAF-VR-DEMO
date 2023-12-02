#pragma once

#include <map>
#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include "MeshBase.h"

// Shaders
static const std::string anime_vertex_shader("skinned_mesh.vert");
static const std::string anime_fragment_shader("skinned_mesh.frag");

class Shader;

class SkinnedMesh : public MeshBase {
protected:
    static Shader* mShader;

public:
    enum UniformLoc : unsigned int {
        PV = 0,
        M = 1,
        Time = 2,
        NumBones = 3,
        Mode = 4,
        DebugID = 5,
        EyeW = 6,
        Shininess = 7,
        UseFlashLight = 8,
        Bones = 20, // array of 100 bones
    };

    enum AttribLoc : unsigned int {
        Pos = 0,
        TexCoord = 1,
        Normal = 2,
        BoneIds = 3,
        BoneWeights = 4,
    };

    SkinnedMesh();
    ~SkinnedMesh() override;

    bool LoadMesh(const std::string& filename) override;

    void Update(float deltaSeconds) override;
    void Render() override;

    [[nodiscard]] static Shader* sShader() { return mShader; }

    [[nodiscard]] unsigned int GetNumBones() const { return m_NumBones; }

    void BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms);

private:
    const static int NUM_BONES_PER_VERTEX = 4;

    struct BoneInfo {
        aiMatrix4x4 BoneOffset;
        aiMatrix4x4 FinalTransformation;

        BoneInfo()
        {
            aiMatrix4x4::Scaling(aiVector3D(0.0f), BoneOffset);
            aiMatrix4x4::Scaling(aiVector3D(0.0f), FinalTransformation);
        }
    };

    struct VertexBoneData {
        unsigned char IDs[NUM_BONES_PER_VERTEX] {};
        float Weights[NUM_BONES_PER_VERTEX] {};

        VertexBoneData()
        {
            memset(IDs, 0, sizeof(IDs));
            memset(Weights, 0, sizeof(Weights));
        };

        void AddBoneData(unsigned int BoneID, float Weight);
    };

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName);
    void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int MeshIndex,
        const aiMesh* paiMesh,
        std::vector<aiVector3D>& Positions,
        std::vector<aiVector3D>& Normals,
        std::vector<aiVector2D>& TexCoords,
        std::vector<VertexBoneData>& Bones,
        std::vector<unsigned int>& Indices);
    void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    enum VB_TYPES : unsigned int {
        INDEX_BUFFER,
        POS_VB,
        NORMAL_VB,
        TEXCOORD_VB,
        BONE_VB,
        NUM_VBs
    };

    GLuint m_VAO = 0;
    GLuint m_Buffers[NUM_VBs] { 0 };

    struct MeshEntry {
        MeshEntry()
        {
            NumIndices = 0;
            BaseVertex = 0;
            BaseIndex = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    static const unsigned int MAX_BONES = 100;

    std::vector<MeshEntry> m_Entries;
    std::vector<GLuint> m_Textures;

    std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
    unsigned int m_NumBones = 0;
    std::vector<BoneInfo> m_BoneInfo;
    aiMatrix4x4 m_GlobalInverseTransform;
    std::vector<aiMatrix4x4> mTransforms;
    std::map<std::string, aiNodeAnim*> mNodeMap;

    void CalcBoundingBox();
};
