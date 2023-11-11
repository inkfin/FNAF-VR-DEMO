#include "StdUniforms.h"

namespace StdUniforms
{

   void MaterialUbo::Init()
   {
      if (mUbo.mBuffer != -1)
      {
         mUbo.Free();
      }
      mUbo.Init(sizeof(MaterialUniforms), &mMaterial);
      mUbo.mBinding = StdUniforms::UboBinding::material;
   }

   void MaterialUbo::Bind()
   {
      if (mUbo.mBuffer != -1)
      {
         mUbo.BindBufferBase();
      }
   }

}