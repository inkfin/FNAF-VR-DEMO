#ifndef __LOADMESH_H__
#define __LOADMESH_H__


#include <string>
#include <vector>
#include <GL/glew.h>
#include "assimp/Scene.h"
#include "assimp/PostProcess.h"

struct SubmeshData
{
   unsigned int mNumIndices;
   unsigned int mBaseIndex;
   unsigned int mBaseVertex;

   SubmeshData() : mNumIndices(0), mBaseIndex(0), mBaseVertex(0) {}
   void DrawSubmesh();
};

struct MeshData
{
   unsigned int mVao;
   unsigned int mVboVerts;
   unsigned int mVboNormals;
   unsigned int mVboTexCoords;
   unsigned int mIndexBuffer;
   float mScaleFactor; //TODO replace with bounding box

   unsigned int mImportFlags = aiProcessPreset_TargetRealtime_Quality | aiProcess_PreTransformVertices;

   const aiScene* mScene;
   aiVector3D mBbMin, mBbMax;

   std::vector<SubmeshData> mSubmesh;
   std::string mFilename;

   MeshData() : mVao(-1), mVboVerts(-1), mVboNormals(-1), mVboTexCoords(-1), mIndexBuffer(-1), mScaleFactor(0.0f), mScene(nullptr) {}
   void FreeMeshData();
   ~MeshData();

   void DrawMesh();

};



MeshData LoadMesh(const std::string& pFile);
bool ValidMeshFilename(const std::string& fname);
void SetMeshDir(std::string dir);
std::string GetMeshDir();

#endif