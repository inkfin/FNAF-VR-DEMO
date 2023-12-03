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
static const std::string title_vertex_shader("title_mesh.vert");
static const std::string title_fragment_shader("title_mesh.frag");

class Shader;

class TitleMesh : public MeshBase {
protected:
    static Shader* mShader;

public:
    enum UniformLoc : unsigned int {
        PV = 0,
        M = 1,
        COLOR = 2,
    };

    enum AttribLoc : unsigned int {
        Pos = 0,
    };

    TitleMesh();
    ~TitleMesh() override;

    bool LoadMesh(const std::string& filename) override;

    void Update(float deltaSeconds) override {};
    void Render() override;

    [[nodiscard]] static Shader* sShader() { return mShader; }

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(const aiMesh* paiMesh,
        std::vector<aiVector3D>& Positions,
        std::vector<unsigned int>& Indices);
    void Clear();

    enum VB_TYPES : unsigned int {
        INDEX_BUFFER,
        POS_VB,
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
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
    };

    std::vector<MeshEntry> m_Entries;

    void CalcBoundingBox();

#undef INVALID_MATERIAL
};
