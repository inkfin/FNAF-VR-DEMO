#include "ReductionPattern.h"

void ImageReductionPattern::Init()
{
   ComputePattern::Init();
}


void ImageReductionPattern::BindData(const ImageTexture& img, int level)
{
   bool layered = false;
   if(img.GetTarget() == GL_TEXTURE_3D) layered = true;
   const int layer = 0;
   //bind mipmap level to c and level+1 to c+nc
   img.SetUnit(0);
   img.BindImageTexture(level, layered, layer, GL_READ_ONLY);
   img.SetUnit(1);
   img.BindImageTexture(level+1, layered, layer, GL_WRITE_ONLY);
}

glm::ivec3 ImageReductionPattern::Reduce(glm::ivec3 size)
{
   return glm::max(size / 2, glm::ivec3(1));
}

ImageTexture ImageReductionPattern::Compute(const ImageTexture& inout)
{
   assert(pShader != nullptr);
   if (mWantsCompute == false) return inout;

   glm::ivec3 size = inout.GetSize();
   glm::ivec3 output_size = Reduce(size);
   int level = 0;

   pShader->UseProgram();
   mTimer.Restart();
   for (;;)
   {
      BindData(inout, level);
      pShader->SetGridSize(output_size);
      pShader->Dispatch();
      glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

      if (output_size == glm::ivec3(1)) break;

      output_size = Reduce(output_size);
      level = level+1;
   }
   mTimer.Stop();
   return inout;
}

ImageTexture ImageReductionPattern::ComputeRect(const ImageTexture& inout, glm::ivec3 min, glm::ivec3 max)
{
    const int offset_loc = 1;
    assert(pShader != nullptr);
    if (mWantsCompute == false) return inout;
    if(glm::any(glm::greaterThan(min, inout.GetSize()))) return inout;
    if (glm::any(glm::lessThan(max, glm::ivec3(0)))) return inout;
    min = glm::clamp(min, glm::ivec3(0), inout.GetSize() - glm::ivec3(1));
    max = glm::clamp(max, glm::ivec3(0), inout.GetSize() - glm::ivec3(1));

    min = min / 2;
    max = max / 2;

    glm::ivec3 output_size = max-min + glm::ivec3(1);
    
    int level = 0;

    pShader->UseProgram();
    mTimer.Restart();
    for (;;)
    {
        BindData(inout, level);
        
        glProgramUniform3iv(pShader->GetShader(), offset_loc, 1, &min.x);
        pShader->SetGridSize(output_size);
        pShader->Dispatch();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        if (level+1 >= inout.GetLevels()) break;

        min = min / 2;
        max = max / 2;
        level = level + 1;

        output_size = max - min + glm::ivec3(1);
    }
    mTimer.Stop();
    return inout;
}

ImageTexture ImageReductionPattern::ComputeProgressive(const ImageTexture& inout, bool start)
{
    assert(pShader != nullptr);
    if (mWantsCompute == false) return inout;

    static glm::ivec3 output_size;
    static int level;

    if (start == true)
    {
        glm::ivec3 size = inout.GetSize();
        output_size = Reduce(size);
        level = 0;
    }

    if(output_size == glm::ivec3(0)) return inout;

    pShader->UseProgram();
    mTimer.Restart();
    //only compute one level
    {
        BindData(inout, level);
        pShader->SetGridSize(output_size);
        pShader->Dispatch();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        if (output_size == glm::ivec3(1))
        {
            output_size = glm::ivec3(0);
        }
        else
        {
            output_size = Reduce(output_size);
        }
        level = level + 1;
    }
    mTimer.Stop();
    return inout;
}

