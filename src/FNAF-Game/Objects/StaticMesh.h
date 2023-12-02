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
static const std::string skinned_vertex_shader("static_mesh.vert");
static const std::string skinned_fragment_shader("static_mesh.frag");

class Shader;

class StaticMesh : public MeshBase {
protected:
    static Shader* mShader;

public:
    enum UniformLoc : unsigned int {
        PV = 0,
        M = 1,
        EyeW = 2,
        Shininess = 3,
        UseFlashLight = 4,
    };

    enum AttribLoc : unsigned int {
        Pos = 0,
        TexCoord = 1,
        Normal = 2,
    };

    StaticMesh();
    ~StaticMesh();

    bool LoadMesh(const std::string& filename) override;

    void Update(float deltaSeconds) override {};
    void Render() override;

    [[nodiscard]] static Shader* sShader() { return mShader; }

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(const aiMesh* paiMesh,
        std::vector<aiVector3D>& Positions,
        std::vector<aiVector3D>& Normals,
        std::vector<aiVector2D>& TexCoords,
        std::vector<unsigned int>& Indices);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    enum VB_TYPES : unsigned int {
        INDEX_BUFFER,
        POS_VB,
        NORMAL_VB,
        TEXCOORD_VB,
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

    std::vector<MeshEntry> m_Entries;
    std::vector<GLuint> m_Textures { INVALID_MATERIAL };

    void CalcBoundingBox();

#undef INVALID_MATERIAL
};
