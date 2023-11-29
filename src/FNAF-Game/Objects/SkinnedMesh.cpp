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

#include "SkinnedMesh.h"

Shader* SkinnedMesh::mShader = nullptr;

void SkinnedMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
    for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; i++) {
        if (Weights[i] == 0.0) {
            IDs[i] = BoneID;
            Weights[i] = Weight;
            return;
        }
    }

    // should never get here - more bones than we have space for
    printf("more bones than we have space for, %d, %p\n", BoneID, this);
    assert(0);
}

SkinnedMesh::SkinnedMesh()
{
    if (mShader == nullptr) {
        mShader = new Shader(anime_vertex_shader, anime_fragment_shader);
        mShader->Init();
    }
}

SkinnedMesh::~SkinnedMesh()
{
    Clear();
}

void SkinnedMesh::Clear()
{

    for (unsigned int i = 0; i < m_Textures.size(); i++) {
        if (m_Textures[i]) {
            glDeleteTextures(1, &m_Textures[i]);
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

void SkinnedMesh::CalcBoundingBox()
{
    mBbMin.x = mBbMin.y = mBbMin.z = 1e10f;
    mBbMax.x = mBbMax.y = mBbMax.z = -1e10f;
    CalcNodeBoundingBox(mScene->mRootNode, mBbMin, mBbMax);

    glm::vec3 diff = mBbMax - mBbMin;
    float w = std::max(diff.x, std::max(diff.y, diff.z));

    mScale = glm::vec3(1.0f / w);
}

bool SkinnedMesh::LoadMesh(const std::string& filename)
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
        m_GlobalInverseTransform = mScene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        ret = InitFromScene(mScene, fullPath);
        CalcBoundingBox();
    } else {
        printf("Error parsing '%s': '%s'\n", fullPath.c_str(), mImporter.GetErrorString());
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);

    return ret;
}

void SkinnedMesh::Update(float deltaSeconds)
{
    BoneTransform(deltaSeconds, mTransforms);
}

void SkinnedMesh::Render()
{
    glUniform1i(UniformLoc::NumBones, m_NumBones);
    glUniformMatrix4fv(UniformLoc::Bones, mTransforms.size(), GL_TRUE, &mTransforms[0].a1);

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

bool SkinnedMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    std::vector<aiVector3D> Positions;
    std::vector<aiVector3D> Normals;
    std::vector<aiVector2D> TexCoords;
    std::vector<VertexBoneData> Bones;
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
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0; i < m_Entries.size(); i++) {
        const aiMesh* pMesh = pScene->mMeshes[i];
        InitMesh(i, pMesh, Positions, Normals, TexCoords, Bones, Indices);
    }

    if (!InitMaterials(pScene, Filename)) {
        return false;
    }

    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), Positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::Pos);
    glVertexAttribPointer(AttribLoc::Pos, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), TexCoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::TexCoord);
    glVertexAttribPointer(AttribLoc::TexCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), Normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::Normal);
    glVertexAttribPointer(AttribLoc::Normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), Bones.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(AttribLoc::BoneIds);
    glVertexAttribIPointer(AttribLoc::BoneIds, 4, GL_UNSIGNED_BYTE, sizeof(VertexBoneData), (const GLvoid*)nullptr);
    glEnableVertexAttribArray(AttribLoc::BoneWeights);
    glVertexAttribPointer(AttribLoc::BoneWeights, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(NUM_BONES_PER_VERTEX * sizeof(unsigned char)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), Indices.data(), GL_STATIC_DRAW);

    return true;
}

void SkinnedMesh::InitMesh(unsigned int MeshIndex,
    const aiMesh* pMesh,
    std::vector<aiVector3D>& Positions,
    std::vector<aiVector3D>& Normals,
    std::vector<aiVector2D>& TexCoords,
    std::vector<VertexBoneData>& Bones,
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
        TexCoords.push_back(aiVector2D(pTexCoord->x, pTexCoord->y));
    }

    LoadBones(MeshIndex, pMesh, Bones);

    // Populate the index buffer
    for (unsigned int i = 0; i < pMesh->mNumFaces; i++) {
        const aiFace& Face = pMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

void SkinnedMesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
    for (unsigned int i = 0; i < pMesh->mNumBones; i++) {
        unsigned int BoneIndex = 0;
        std::string BoneName(pMesh->mBones[i]->mName.data);

        auto iter = m_BoneMapping.find(BoneName);
        if (iter == m_BoneMapping.end()) {
            // Allocate an index for a new bone
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
            m_BoneMapping[BoneName] = BoneIndex;
        } else {
            BoneIndex = iter->second;
        }

        for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

bool SkinnedMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
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

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string p(Path.data);

                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                std::string FullPath = Dir + "/" + p;

                m_Textures[i] = LoadTexture(FullPath);
                if (m_Textures[i] == -1) {
                    Ret = false;
                }
            }
        }
    }

    return Ret;
}

unsigned int SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

unsigned int SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

unsigned int SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    const unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    const unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    const float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    const float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    // assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    const aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    const unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    const unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    const float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    const float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    // assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    const unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    const unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    const float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    const float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    // assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    const aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void SkinnedMesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform)
{
    const std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = mScene->mAnimations[0];

    aiMatrix4x4 NodeTransformation(pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        aiMatrix4x4 ScalingM;
        aiMatrix4x4::Scaling(Scaling, ScalingM);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        aiMatrix4x4 TranslationM;
        aiMatrix4x4::Translation(Translation, TranslationM);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

    auto iter = m_BoneMapping.find(NodeName);
    if (iter != m_BoneMapping.end()) {
        const unsigned int BoneIndex = iter->second;
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void SkinnedMesh::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms)
{
    aiMatrix4x4 Identity;

    if (mScene->mNumAnimations == 0) {
        return;
    }

    float TicksPerSecond = (float)(mScene->mAnimations[0]->mTicksPerSecond != 0 ? mScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)mScene->mAnimations[0]->mDuration);

    ReadNodeHierarchy(AnimationTime, mScene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for (unsigned int i = 0; i < m_NumBones; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName)
{
    auto it = mNodeMap.find(NodeName);
    if (it != mNodeMap.end()) {
        return it->second;
    }

    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            // insert in map, so next Find will be fast
            mNodeMap[NodeName] = pNodeAnim;
            return pNodeAnim;
        }
    }
    mNodeMap[NodeName] = nullptr;
    return nullptr;
}
