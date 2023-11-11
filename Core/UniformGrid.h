#pragma once
#include "ComputePattern.h"
#include "Aabb.h"

struct GridInfo2D
{
   aabb2D mExtents;
   glm::ivec2 mNumCells = glm::ivec2(-1);
   glm::vec2 mCellSize = glm::vec2(-1.0f);
   int mMaxCellsPerElement = 1;
};

class UniformGrid2D
{
   public:

      UniformGrid2D();
      void Init();

      void UpdateGrid(BufferArray elements);
      void SetNumElements(int n) {mNumElements = n;}
      void SetMaxElements(int n) {mMaxElements = n;}
      void SetUniformGridShader(ComputeShader& shader) { pUniformGridShader = &shader;}
      void SetMaxCellsPerElement(int m) { mGridInfo.mMaxCellsPerElement = m;}

      GridInfo2D GetGridInfo() {return mGridInfo;}
      void SetGridInfo(GridInfo2D grid) { mGridInfo = grid;}
      void BindGridInfoUbo();
      BufferArray GetCount() {return mCount;}
      BufferArray GetStart() {return mStart;}
      BufferArray GetIndex() {return mIndex;}

   protected:
   
      GpuTimer mTimer;

      GridInfo2D mGridInfo;
      Buffer mGridInfoUbo = Buffer(GL_UNIFORM_BUFFER);

      BufferArray mCount = BufferArray(GL_SHADER_STORAGE_BUFFER);
      BufferArray mStart = BufferArray(GL_SHADER_STORAGE_BUFFER);
      BufferArray mIndex = BufferArray(GL_SHADER_STORAGE_BUFFER);
      int mNumElements = -1; //number of points or boxes, etc.
      int mMaxElements = -1;

      ComputeShader mParallelScanShader = ComputeShader("prefix_sum_cs.glsl");
      BufferScan mParallelScan;

      ComputeShader* pUniformGridShader = nullptr;
      std::string mDefaultGridShaderName = "uniform_grid_points_2d_cs.glsl";
      ComputeShader* pDefaultGridShader = nullptr;
      
      const int kGridUboBinding = 0;

      const int kElementsBinding = 0;
      const int kCountBinding = 2;
      const int kStartBinding = 3;
      const int kIndexBinding = 4;
      

      enum eModes : int
      {
         eComputeCount = 0,
         eComputeStart = 1,
         eInsertContents = 2   
      };
};


struct GridInfo3D
{
   aabb3D mExtents;
   glm::ivec4 mNumCells = glm::ivec4(-1);
   glm::vec4 mCellSize = glm::vec4(-1.0f);
   int mMaxCellsPerElement = 1;
};

class UniformGrid3D
{
   public:

      UniformGrid3D();
      UniformGrid3D(aabb3D extents, glm::ivec3 num_cells);
      void Init();

      void UpdateGrid(BufferArray elements);
      void SetNumElements(int n) { mNumElements = n; }
      void SetMaxElements(int n) {mMaxElements = n;}
      void SetUniformGridShader(ComputeShader& shader) { pUniformGridShader = &shader; }
      void SetMaxCellsPerElement(int m) { mGridInfo.mMaxCellsPerElement = m; }

      GridInfo3D GetGridInfo() { return mGridInfo; }
      void SetGridInfo(GridInfo3D grid) { mGridInfo = grid;}
      void BindGridInfoUbo();
      BufferArray GetCount() { return mCount; }
      BufferArray GetStart() { return mStart; }
      BufferArray GetIndex() { return mIndex; }

   protected:

      GpuTimer mTimer;

      GridInfo3D mGridInfo;
      Buffer mGridInfoUbo = Buffer(GL_UNIFORM_BUFFER);

      BufferArray mCount = BufferArray(GL_SHADER_STORAGE_BUFFER);
      BufferArray mStart = BufferArray(GL_SHADER_STORAGE_BUFFER);
      BufferArray mIndex = BufferArray(GL_SHADER_STORAGE_BUFFER);
      int mNumElements = -1; //number of points or boxes, etc.
      int mMaxElements = -1;

      ComputeShader mParallelScanShader = ComputeShader("prefix_sum_cs.glsl");
      BufferScan mParallelScan;

      ComputeShader* pUniformGridShader = nullptr;
      std::string mDefaultGridShaderName = "uniform_grid_points_2d_cs.glsl";
      ComputeShader* pDefaultGridShader = nullptr;

      const int kGridUboBinding = 0;

      const int kElementsBinding = 0;
      const int kCountBinding = 2;
      const int kStartBinding = 3;
      const int kIndexBinding = 4;


      enum eModes : int
      {
         eComputeCount = 0,
         eComputeStart = 1,
         eInsertContents = 2
      };
};