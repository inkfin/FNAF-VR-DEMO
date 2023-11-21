#pragma once

#include "Module.h"
#include "ImageTexture.h"
#include "ReductionPattern.h"
#include "StdUniforms.h"
#include "Shader.h"

#include <glm/gtx/transform.hpp>

struct RaytraceGrid2D: public Module
{
   ImageMap image_map;
   ImageTexture mGrid = ImageTexture(GL_TEXTURE_2D);
   ComputeShader mShader = ComputeShader("raytrace_grid_2d_cs.glsl");

   void Init() override;
   void Compute() override;
};


struct RaytraceGrid3D: public Module
{
   ImageTexture mVoxels = ImageTexture(GL_TEXTURE_3D);
   ImageTexture mImage = ImageTexture(GL_TEXTURE_2D);

   ImageMap generator_map;
   ComputeShader mGenShader = ComputeShader("generate_grid_3d_cs.glsl");

   ImageMap raytrace_map;
   ComputeShader mRaytraceShader = ComputeShader("raytrace_grid_3d_cs.glsl");

   void Init() override;
   //void Compute() override;
   void GenerateVoxels();
   void GenerateImage();
};

struct RaytraceOctreeGrid3D: public Module
{
   ImageTexture mVoxels = ImageTexture(GL_TEXTURE_3D);
   ImageTexture mImage = ImageTexture(GL_TEXTURE_2D);

   ImageMap generator_map;
   ComputeShader mGenShader = ComputeShader("generate_grid_3d_cs.glsl");

   ImageMap raytrace_map;
   ComputeShader mRaytraceShader = ComputeShader("raytrace_octree_grid_3d_cs.glsl");

   ImageReductionPattern mipmap_reduce;
   ComputeShader mMipmapShader = ComputeShader("image_reduce_3d_cs.glsl");

   //For rendering to fbo instead of mImage
   Shader mRaytraceFragShader = Shader("raytrace_octree_grid_3d_vs.glsl", "raytrace_octree_grid_3d_fs.glsl");

   Buffer mRaytraceBuffer = Buffer(GL_SHADER_STORAGE_BUFFER);

   struct RaytraceData
   {
	   glm::vec4 ray_intersection[2];
	   glm::ivec4 dirty_rect_min[2];
	   glm::ivec4 dirty_rect_max[2];
   };

   RaytraceData mRaytraceData;

   glm::ivec2 mImageSize;

   glm::mat4 mMcen;
   glm::mat4 mM;
   glm::mat4 mMinv;

   glm::vec3 mPos = glm::vec3(0.0f, 1.0f, 0.0f);
   glm::vec3 mScale = glm::vec3(1.0f);
   float mAngle = 0.0f;

   void UpdateM();
   int mPV_inv_loc = -1;
   int mM_inv_loc = -1;
   int mM_loc = -1;


   StdUniforms::MaterialUbo mMaterialUbo;

   void Init() override;
   //void Compute() override;
   void GenerateVoxels();
   void GenerateImage(const glm::mat4& P, const glm::mat4& V);
   void Draw(const glm::mat4& P, const glm::mat4& V);
   void SetImageSize(glm::ivec2 size) { mImageSize = size; }
   void GenerateMipmap();

   glm::vec3 GetScale() { return mScale; }
   void SetScale(glm::vec3 s) { mScale = s; UpdateM(); }
   glm::vec3 GetPos() { return mPos; }
   void SetPos(glm::vec3 p) { mPos = p; UpdateM();}
   float GetAngle() { return mAngle; }
   void SetAngle(float a) { mAngle = a; UpdateM();}
   void SetColor(glm::vec4 color);
   void SetBrushXform(const glm::mat3& M);
   void SetBrushShape(int shape);

   void Subtract(int hand, glm::vec3 pos, float r);
   void Add(int hand, glm::vec3 pos, float r);
   void Paint(int hand, glm::vec3 pos, float r);

   void Subtract(int hand, glm::vec3 ro, glm::vec3 rd, float r);
   void Add(int hand, glm::vec3 ro, glm::vec3 rd, float r);
   void Paint(int hand, glm::vec3 ro, glm::vec3 rd, float r);

};
