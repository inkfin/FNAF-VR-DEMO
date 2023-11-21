#include "ComputePattern.h"
#include <cassert>


void ComputePattern::Init()
{
   assert(pShader != nullptr);
   pShader->Init();
}

void ComputePattern::SetComputeShader(ComputeShader& cs)
{
   pShader = &cs;
   mTimer.SetName(pShader->GetName());
}

void ComputePattern::Compute()
{
   assert(pShader != nullptr);

   if (mWantsCompute == false) return;

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {

      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   }
   mTimer.Stop();
}

/////////////////////////////////////////////
// BufferMap

BufferArray BufferMap::Compute(const BufferArray& inout)
{
   assert(pShader != nullptr);

   if (mWantsCompute == false) return inout;

   pShader->SetNumElements(inout.mNumElements);
   pShader->SetGridSize(glm::ivec3(inout.mNumElements, 1, 1));

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      inout.BindBuffer(kInputOutputBinding);

      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   }
   mTimer.Stop();
   return inout;
}

BufferArray BufferMap::Compute(BufferArray in, BufferArray out)
{
   assert(pShader != nullptr);
   assert(in.mBuffer != out.mBuffer); //stencil cannot be in-place

   if (mWantsCompute == false) return in;

   pShader->SetNumElements(out.mNumElements);
   pShader->SetGridSize(glm::ivec3(out.mNumElements, 1, 1));

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      if (in.mBuffer == out.mBuffer)
      {
         in.BindBuffer(kInputBinding);
      }
      else
      {
         if (i > 1) std::swap<Buffer>(in, out); //ping pong
         in.BindBuffer(kInputBinding);
         out.BindBuffer(kOutputBinding);
      }

      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

   }
   mTimer.Stop();
   return out;
}

////////////////////////////////
/// BufferStencil

void BufferStencil::Compute(std::vector<BufferArray>& buffers)
{
   int nv = buffers.size();
   assert(pShader != nullptr);

   if (mWantsCompute == false) return;

   pShader->SetNumElements(buffers[0].mNumElements);
   pShader->SetGridSize(glm::ivec3(buffers[0].mNumElements, 1, 1));

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      for (int v = 0; v < nv; v++)
      {
         buffers[v].BindBuffer(v);
      }

      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
 
      for (int v = 0; v < nv - 1; v++)
      {
         std::swap<Buffer>(buffers[v], buffers[v + 1]); //ping pong
      }
   }
   mTimer.Stop();
}

BufferArray BufferStencil::Compute(BufferArray in, BufferArray out)
{
   assert(pShader != nullptr);
   assert(in.mBuffer != out.mBuffer); //stencil cannot be in-place

   if (mWantsCompute == false) return in;

   pShader->SetNumElements(out.mNumElements);
   pShader->SetGridSize(glm::ivec3(out.mNumElements, 1, 1));

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      if (in.mBuffer == out.mBuffer)
      {
         in.BindBuffer(kInputBinding);
      }
      else
      {
         if(i>1) std::swap<Buffer>(in, out); //ping pong
         in.BindBuffer(kInputBinding);
         out.BindBuffer(kOutputBinding);
      }

      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
      
   }
   mTimer.Stop();
   return out;
}


///////////////////////////////
/// Odd-Even Sort

void BufferOddEvenSort::Init()
{
   int flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
   mSorted.Init(sizeof(int), nullptr, flags);
   pSorted = (int*)glMapNamedBufferRange(mSorted.mBuffer, 0, sizeof(int), flags);
   pSorted[0] = false;

}

BufferArray BufferOddEvenSort::Compute(const BufferArray& inout)
{
   const int MODE_EVEN = 0;
   const int MODE_ODD = 1;

   assert(pShader != nullptr);
   //assert(mSubsteps == 1); //substeps don't make sense here
   if (mWantsCompute == false) return inout;

   const int M = 1;
   int n = inout.mNumElements/(2*M) + 1;

   pShader->SetNumElements(inout.mNumElements);
   pShader->SetGridSize(glm::ivec3(n, 1, 1));

   inout.BindBuffer(kInputOutputBinding);
   mSorted.BindBuffer(1);

   pShader->UseProgram();
   mTimer.Restart();

   mSubsteps = 100;
   GLsync sync=0;
   int iteration = 0;
   do
   {
      pSorted[0] = true;
      for(int i=0; i<mSubsteps; i++)
      {
         pShader->SetMode(MODE_ODD);
         pShader->Dispatch();
         glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

         pShader->SetMode(MODE_EVEN);
         pShader->Dispatch();
         if (i==mSubsteps*0.9)
         {
            sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
         }
         glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
         iteration++;
      }
      glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
      GLenum result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1e6);
      glDeleteSync(sync);
      
   }while(pSorted[0] == false);

   mTimer.Stop();
   return inout;
};

///////////////////////////////
/// BitonicSort

BufferArray BufferBitonicSort::Compute(const BufferArray& inout)
{
   assert(pShader != nullptr);
   assert(mSubsteps == 1); //substeps don't make sense here

   if (mWantsCompute == false) return inout;

   int n = inout.mNumElements;
   assert(((n & (n - 1)) == 0)); //n must be a power of 2

   pShader->UseProgram();
   mTimer.Restart();

   inout.BindBuffer(0);

   uint32_t max_workgroup_size = pShader->GetWorkGroupSize().x;
   uint32_t workgroup_size_x = 1;

   // Adjust workgroup_size_x to get as close to max_workgroup_size as possible.
   if (n < max_workgroup_size * 2)
   {
      workgroup_size_x = n / 2;
   }
   else
   {
      workgroup_size_x = max_workgroup_size;
   }

   int workgroup_count = n / (workgroup_size_x * 2);

   uint32_t h = workgroup_size_x * 2;
   assert(h <= n);
   assert(h % 2 == 0);

   Dispatch(workgroup_count, h, eAlgorithmVariant::eLocalBitonicMergeSortExample);
   // we must now double h, as this happens before every flip
   h *= 2;
   for (; h <= n; h *= 2)
   {
      Dispatch(workgroup_count, h, eAlgorithmVariant::eBigFlip);
      for (uint32_t hh = h / 2; hh > 1; hh /= 2)
      {
         if (hh <= workgroup_size_x * 2)
         {
            // We can fit all elements for a disperse operation into continuous shader workgroup local memory, 
            // which means we can complete the rest of the cascade using a single shader invocation.
            Dispatch(workgroup_count, hh, eAlgorithmVariant::eLocalDisperse);
            break;
         }
         else
         {
            Dispatch(workgroup_count, hh, eAlgorithmVariant::eBigDisperse);
         }
      }
   }
   mTimer.Stop();
   return inout;
}

void BufferBitonicSort::Dispatch(int num_workgroups, int h, int alg)
{
   const GLuint shader = pShader->GetShader();
   glProgramUniform1i(shader, 0, h);
   glProgramUniform1i(shader, 1, alg);
   glDispatchCompute(num_workgroups, 1, 1);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void BufferScan::Init()
{
   ComputePattern::Init();
   mStrideLoc = pShader->GetUniformLocation("stride");
   assert(mStrideLoc != -1);
}

/////////////////////////
/// BufferScan

BufferArray BufferScan::Compute(const BufferArray& inout)
{
   assert(pShader != nullptr);
   assert(mSubsteps == 1); //substeps don't make sense here

   if (mWantsCompute == false) return inout;

   int n = inout.mNumElements;
   assert(((n & (n - 1)) == 0)); //n must be a power of 2
   pShader->SetNumElements(inout.mNumElements);
   pShader->UseProgram();
   mTimer.Restart();
   inout.BindBuffer(kInputOutputBinding);
   
   //upsweep is a reduction
   pShader->SetMode(eUpsweep);
   n = n / 2;
   int pass = 0;
   for (;;)
   {
      int stride = 2 << pass;
      glProgramUniform1i(pShader->GetShader(), mStrideLoc, stride); //STRIDE = 2, 4, 8, ...
      pShader->SetGridSize(glm::ivec3(n, 1, 1));
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      if (n == 1) break;
      n = n / 2;
      pass = pass + 1;
   }
   
   //downsweep
   pShader->SetMode(eDownsweep);
   for (;;)
   {
      int stride = 2 << pass;
      glProgramUniform1i(pShader->GetShader(), mStrideLoc, stride); //STRIDE = ...
      pShader->SetGridSize(glm::ivec3(n, 1, 1));
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

      if (n == inout.mNumElements / 2) break;
      n = n * 2;
      pass = pass - 1;
   }
   mTimer.Stop();
   return inout;
}

//////////////////////
/// ImageMap

ImageTexture ImageMap::Compute(const ImageTexture& inout)
{
   assert(pShader != nullptr);
   if (mWantsCompute == false) return inout;

   inout.BindImageTexture(kInputOutputBinding, GL_READ_WRITE);

   pShader->SetGridSize(inout.GetSize());

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
   }
   mTimer.Stop();
   return inout;
}

ImageTexture ImageMap::Compute(ImageTexture in, ImageTexture out)
{
   assert(pShader != nullptr);
   if (mWantsCompute == false) return out;

   pShader->SetGridSize(in.GetSize());

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      if (in.GetTexture() == out.GetTexture())
      {
         in.BindImageTexture(kInputOutputBinding, GL_READ_WRITE);
      }
      else
      {
         if (i > 1) std::swap<ImageTexture>(in, out); //ping pong
         in.BindImageTexture(kInputBinding, GL_READ_ONLY);
         out.BindImageTexture(kOutputBinding, GL_WRITE_ONLY);
      }
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
   }
   mTimer.Stop();
   return out;
}

ImageTexture ImageMap::ComputeRect(const ImageTexture& inout, glm::ivec3 min, glm::ivec3 max)
{
    assert(pShader != nullptr);
    if (mWantsCompute == false) return inout;

    inout.BindImageTexture(kInputOutputBinding, GL_READ_WRITE);

    pShader->SetGridSize(max - min + glm::ivec3(1));

    pShader->UseProgram();
    mTimer.Restart();
    for (int i = 0; i < mSubsteps; i++)
    {
        pShader->Dispatch();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }
    mTimer.Stop();
    return inout;
}

///////////////////////////
/// ImageStencil

void ImageStencil::Compute(std::vector<ImageTexture>& images)
{
   //images: last is write, the rest are read
   int nv = images.size();
   assert(pShader != nullptr);
   if (mWantsCompute == false) return;

   pShader->SetGridSize(images[0].GetSize());

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      for(int v=0; v<nv-1; v++)
      {
         images[v].BindImageTexture(v, GL_READ_ONLY);
      }
      images[nv-1].BindImageTexture(nv-1, GL_WRITE_ONLY);

      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
      for (int v = 0; v < nv - 1; v++)
      {
         std::swap<ImageTexture>(images[v], images[v+1]); //ping pong
      }
   }
   mTimer.Stop();

}

ImageTexture ImageStencil::Compute(ImageTexture in, ImageTexture out)
{
   assert(pShader != nullptr);
   if (mWantsCompute == false) return out;

   pShader->SetGridSize(in.GetSize());

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      if (in.GetTexture() == out.GetTexture())
      {
         in.BindImageTexture(kInputOutputBinding, GL_READ_WRITE);
      }
      else
      {
         if(i>1) std::swap<ImageTexture>(in, out); //ping pong
         in.BindImageTexture(kInputBinding, GL_READ_ONLY);
         out.BindImageTexture(kOutputBinding, GL_WRITE_ONLY);
      }
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
   }
   mTimer.Stop();
   return out;
}


///////////////////////////////
/// Odd-Even Sort

void ImageOddEvenSort::Init()
{
   int flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
   mSorted.Init(sizeof(int), nullptr, flags);
   pSorted = (int*)glMapNamedBufferRange(mSorted.mBuffer, 0, sizeof(int), flags);
   pSorted[0] = false;

}

ImageTexture ImageOddEvenSort::Compute(const ImageTexture& inout)
{
   const int MODE_EVEN = 0;
   const int MODE_ODD = 1;

   assert(pShader != nullptr);
   //assert(mSubsteps == 1); //substeps don't make sense here
   if (mWantsCompute == false) return inout;

   glm::ivec3 grid = inout.GetSize() / 2 + glm::ivec3(1,1,0);
   pShader->SetGridSize(grid);

   inout.BindImageTexture(kInputOutputBinding, GL_READ_WRITE);
   mSorted.BindBuffer(1);

   pShader->UseProgram();
   mTimer.Restart();

   mSubsteps = 100;
   GLsync sync=0;
   int iteration = 0;
   do
   {
      pSorted[0] = true;
      for (int i = 0; i < mSubsteps; i++)
      {
         pShader->SetMode(MODE_ODD);
         pShader->Dispatch();
         glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

         pShader->SetMode(MODE_EVEN);
         pShader->Dispatch();
         if (i == mSubsteps * 0.9)
         {
            sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
         }
         glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
         iteration++;
      }
      glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
      GLenum result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1e6);
      glDeleteSync(sync);

   } while (pSorted[0] == false);

   mTimer.Stop();
   return inout;
};

ImageTexture ImageOddEvenSort::ComputeN(const ImageTexture& inout)
{
   const int MODE_EVEN = 0;
   const int MODE_ODD = 1;

   assert(pShader != nullptr);
   if (mWantsCompute == false) return inout;

   glm::ivec3 grid = inout.GetSize();///glm::ivec3(2,1,1) + glm::ivec3(1, 0, 0);
   pShader->SetGridSize(grid);

   inout.BindImageTexture(kInputOutputBinding, GL_READ_WRITE);
   mSorted.BindBuffer(1);

   pShader->UseProgram();
   mTimer.Restart();

   for (int i = 0; i < mSubsteps; i++)
   {
      pShader->SetMode(MODE_ODD);
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      pShader->SetMode(MODE_EVEN);
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
   }

   mTimer.Stop();
   return inout;
};


////////////////////////
/// ImageToBufferPattern

BufferArray ImageToBufferPattern::Compute(const ImageTexture& in, const BufferArray& out)
{
   assert(pShader != nullptr);
   if (mWantsCompute == false) return out;
   
   pShader->SetNumElements(out.mNumElements);
   pShader->SetGridSize(glm::ivec3(out.mNumElements, 1, 1));

   in.BindImageTexture(kInputBinding, GL_READ_ONLY);
   pShader->SetGridSize(in.GetSize());

   out.BindBuffer(kOutputBinding);

   pShader->UseProgram();
   mTimer.Restart();
   for (int i = 0; i < mSubsteps; i++)
   {
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   }
   mTimer.Stop();
   return out;
}
