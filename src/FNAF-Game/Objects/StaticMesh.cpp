// Based on the tutorial at
// https://ogldev.org/www/tutorial38/tutorial38.html

// Modifications
//  Speed up FindAnimNode with std::map
//  Use aiProcess_LimitBoneWeights when importing, to limit bones per vertex to 4
//  Use unsigned byte for bone IDs
//  Use layout qualifiers for attribs and uniforms
//  Eliminate SetBoneTransform() - send all matrices in one glUniform call
//  Pass strings by reference

#include <cassert>

#include "LoadTexture.h"
#include "Shader.h"

#include "StaticMesh.h"

Shader* StaticMesh::mShader = nullptr;

StaticMesh::StaticMesh()
{
    if (mShader == nullptr) {
        mShader = new Shader(skinned_vertex_shader, skinned_fragment_shader);
        mShader->Init();
    }
}

StaticMesh::~StaticMesh()
{
    Clear();
}

void StaticMesh::Clear()
{
    for (unsigned int& m_Texture : m_Textures) {
        if (m_Texture) {
            glDeleteTextures(1, &m_Texture);
        }
    }

    if (m_Buffers[0] != 0) {
        glDeleteBuffers(NUM_VBs, m_Buffers);
    }

    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}

void StaticMesh::CalcBoundingBox()
{
    mBbMin.x = mBbMin.y = mBbMin.z = 1e10f;
    mBbMax.x = mBbMax.y = mBbMax.z = -1e10f;
    CalcNodeBoundingBox(mScene->mRootNode, mBbMin, mBbMax);

    glm::vec3 diff = mBbMax - mBbMin;
    float w = std::max(diff.x, std::max(diff.y, diff.z));

    mScale = glm::vec3(1.0f / w);
}

bool StaticMesh::LoadMesh(const std::string& filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    // Create the VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    glGenBuffers(NUM_VBs, m_Buffers);

    bool ret = false;

    std::string fullPath = filename;

    std::replace(fullPath.begin(), fullPath.end(), '/', '\\');

    // aiProcessPreset_TargetRealtime_Quality includes aiProcess_LimitBoneWeights which restricts bones per vertex to 4
    mScene = mImporter.ReadFile(fullPath.c_str(), aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

    if (mScene) {
        printf("Loading mesh %s\n", fullPath.c_str());
        ret = InitFromScene(mScene, fullPath);
        CalcBoundingBox();
    } else {
        printf("Error parsing '%s': '%s'\n", fullPath.c_str(), mImporter.GetErrorString());
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);

    return ret;
}

bool StaticMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    std::vector<aiVector3D> Positions;
    std::vector<aiVector3D> Normals;
    std::vector<aiVector2D> TexCoords;
    std::vector<unsigned int> Indices;

    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;

    // Count the number of vertices and indices
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;

        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += m_Entries[i].NumIndices;
    }

    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* pMesh = pScene->mMeshes[i];
        InitMesh(pMesh, Positions, Normals, TexCoords, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::Pos);
    glVertexAttribPointer(AttribLoc::Pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::TexCoord);
    glVertexAttribPointer(AttribLoc::TexCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::Normal);
    glVertexAttribPointer(AttribLoc::Normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    return true;
}

void StaticMesh::InitMesh(const aiMesh* pMesh,
    std::vector<aiVector3D>& Positions,
    std::vector<aiVector3D>& Normals,
    std::vector<aiVector2D>& TexCoords,
    std::vector<unsigned int>& Indices)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for (unsigned int i = 0; i < pMesh->mNumVertices; i++) {
        const aiVector3D* pPos = &(pMesh->mVertices[i]);
        const aiVector3D* pNormal = &(pMesh->mNormals[i]);
        const aiVector3D* pTexCoord = pMesh->HasTextureCoords(0) ? &(pMesh->mTextureCoords[0][i]) : &Zero3D;

        Positions.push_back(*pPos);
        Normals.push_back(*pNormal);
        TexCoords.emplace_back(pTexCoord->x, pTexCoord->y);
    }

    // Populate the index buffer
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
        const aiFace& Face = pMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

bool StaticMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of('\\');
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    } else if (SlashIndex == 0) {
        Dir = "\\";
    } else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;

        aiColor4D color(0.f, 0.f, 0.f, 0.0f);
        aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS) {
                std::string p(Path.data);

                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                std::string FullPath = Dir + "\\" + p;

                m_Textures[i] = LoadTexture(FullPath);
                if (m_Textures[i] == -1) {
                    Ret = false;
                }
            }
        }
    }

    return Ret;
}

void StaticMesh::Render()
{
    glBindVertexArray(m_VAO);

    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

        assert(MaterialIndex < m_Textures.size());

        if (m_Textures[MaterialIndex]) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_Textures[MaterialIndex]);
        }

        glDrawElementsBaseVertex(GL_TRIANGLES,
            m_Entries[i].NumIndices,
            GL_UNSIGNED_INT,
            (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
            m_Entries[i].BaseVertex);
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}
