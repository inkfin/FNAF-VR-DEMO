#include "UniformGrid.h"

UniformGrid2D::UniformGrid2D():mTimer("UniformGrid2D::UpdateGrid", false)
{
   const glm::ivec2 num_cells(16,16);
   const aabb2D extents(glm::vec2(-1.0f), glm::vec2(+1.0f));

   mGridInfo.mNumCells = num_cells;
   mGridInfo.mExtents = extents;
   mGridInfo.mCellSize = (mGridInfo.mExtents.mMax - mGridInfo.mExtents.mMin) / glm::vec2(mGridInfo.mNumCells);
}


void UniformGrid2D::Init()
{
   if (pUniformGridShader == nullptr)
   {
      pDefaultGridShader = new ComputeShader(mDefaultGridShaderName);
      pUniformGridShader = pDefaultGridShader;
   }
   pUniformGridShader->Init();

   mParallelScan.SetComputeShader(mParallelScanShader);
   mParallelScan.Init();

   assert(mNumElements <= mMaxElements);

   mIndex.Init(mGridInfo.mMaxCellsPerElement*mMaxElements, sizeof(int));
   mIndex.mNumElements = mNumElements;

   int num_cells = mGridInfo.mNumCells.x * mGridInfo.mNumCells.y;
   mCount.Init(num_cells, sizeof(int));
   mStart.Init(num_cells, sizeof(int));

   mGridInfo.mCellSize = (mGridInfo.mExtents.mMax - mGridInfo.mExtents.mMin) / glm::vec2(mGridInfo.mNumCells);
   mGridInfoUbo.Init(sizeof(GridInfo2D), &mGridInfo);
}

void UniformGrid2D::BindGridInfoUbo()
{
   mGridInfoUbo.BindBufferBase(kGridUboBinding);
}

void UniformGrid2D::UpdateGrid(BufferArray points)
{
   assert(pUniformGridShader != nullptr);
   //TODO: optimize
   pUniformGridShader->UseProgram();
   mTimer.Restart();
   points.BindBufferBase(kElementsBinding);
   mGridInfoUbo.BindBufferBase(kGridUboBinding);
   mCount.BindBufferBase(kCountBinding);
   mStart.BindBufferBase(kStartBinding);
   mIndex.BindBufferBase(kIndexBinding);
   
   assert(mNumElements == points.mNumElements);
   pUniformGridShader->SetNumElements(points.mNumElements);
   pUniformGridShader->SetGridSize(glm::ivec3(points.mNumElements, 1, 1));

   //clear counter
   mCount.ClearToInt(0);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //Compute Count
   pUniformGridShader->SetMode(eComputeCount);
   pUniformGridShader->Dispatch();
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //Compute Start (prefix sum)
   //TODO eliminate copy by accumulating count into mStart and computing start in-place
   mStart.CopyFromBufferSubData(mCount);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   mParallelScan.Compute(mStart);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //clear counter
   mCount.ClearToInt(0);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //Insert points
   pUniformGridShader->UseProgram();
   points.BindBufferBase(kElementsBinding);
   mGridInfoUbo.BindBufferBase(kGridUboBinding);
   mCount.BindBufferBase(kCountBinding);
   mStart.BindBufferBase(kStartBinding);
   mIndex.BindBufferBase(kIndexBinding);
   
   pUniformGridShader->SetMode(eInsertContents);
   pUniformGridShader->Dispatch();
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   mTimer.Stop();
}




///////////////////////////////////////////////////////////////////////////////////




UniformGrid3D::UniformGrid3D() :mTimer("UniformGrid3D::UpdateGrid", false)
{
   const glm::ivec3 num_cells(16, 16, 16);
   const aabb3D extents(glm::vec3(-1.0f), glm::vec3(+1.0f));

   mGridInfo.mNumCells = glm::ivec4(num_cells, 1);
   mGridInfo.mExtents = extents;
   mGridInfo.mCellSize = (mGridInfo.mExtents.mMax - mGridInfo.mExtents.mMin) / glm::vec4(mGridInfo.mNumCells);
}

UniformGrid3D::UniformGrid3D(aabb3D extents, glm::ivec3 num_cells) :mTimer("UniformGrid3D::UpdateGrid", false)
{
   mGridInfo.mNumCells = glm::ivec4(num_cells, 1);
   mGridInfo.mExtents = extents;
   mGridInfo.mCellSize = (mGridInfo.mExtents.mMax - mGridInfo.mExtents.mMin) / glm::vec4(mGridInfo.mNumCells);
}

void UniformGrid3D::Init()
{
   if (pUniformGridShader == nullptr)
   {
      pDefaultGridShader = new ComputeShader(mDefaultGridShaderName);
      pUniformGridShader = pDefaultGridShader;
   }
   pUniformGridShader->Init();

   mParallelScan.SetComputeShader(mParallelScanShader);
   mParallelScan.Init();

   assert(mNumElements <= mMaxElements);

   mIndex.Init(mGridInfo.mMaxCellsPerElement*mMaxElements, sizeof(int));

   int num_cells = mGridInfo.mNumCells.x * mGridInfo.mNumCells.y * mGridInfo.mNumCells.z;
   mCount.Init(num_cells, sizeof(int));
   mStart.Init(num_cells, sizeof(int));

   mGridInfo.mCellSize = (mGridInfo.mExtents.mMax - mGridInfo.mExtents.mMin) / glm::vec4(mGridInfo.mNumCells);
   mGridInfoUbo.Init(sizeof(GridInfo3D), &mGridInfo);
}

void UniformGrid3D::BindGridInfoUbo()
{
   mGridInfoUbo.BindBufferBase(kGridUboBinding);
}

void UniformGrid3D::UpdateGrid(BufferArray points)
{
   assert(pUniformGridShader != nullptr);
   //TODO: optimize
   pUniformGridShader->UseProgram();

   //mTimer.Restart();
   points.BindBufferBase(kElementsBinding);
   mGridInfoUbo.BindBufferBase(kGridUboBinding);
   mCount.BindBufferBase(kCountBinding);
   mStart.BindBufferBase(kStartBinding);
   mIndex.BindBufferBase(kIndexBinding);

   assert(mNumElements == points.mNumElements);
   pUniformGridShader->SetNumElements(points.mNumElements);
   pUniformGridShader->SetGridSize(glm::ivec3(points.mNumElements, 1, 1));

   //clear counter
   mCount.ClearToInt(0);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //Compute Count
   pUniformGridShader->SetMode(eComputeCount);
   pUniformGridShader->Dispatch();
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //Compute Start (prefix sum)
   //TODO eliminate copy by accumulating count into mStart and computing start in-place
   mStart.CopyFromBufferSubData(mCount);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   mParallelScan.Compute(mStart);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //clear counter
   mCount.ClearToInt(0);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   //Insert points
   pUniformGridShader->UseProgram();
   points.BindBufferBase(kElementsBinding);
   mGridInfoUbo.BindBufferBase(kGridUboBinding);
   mCount.BindBufferBase(kCountBinding);
   mStart.BindBufferBase(kStartBinding);
   mIndex.BindBufferBase(kIndexBinding);

   pUniformGridShader->SetMode(eInsertContents);
   pUniformGridShader->Dispatch();
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   //mTimer.Stop();
}