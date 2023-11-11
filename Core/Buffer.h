#pragma once

#include <GL/glew.h>

class Buffer
{
   public:
      GLuint mBuffer = -1;
      mutable GLuint mBinding = -1;
      GLenum mTarget = -1;
      GLuint mFlags = 0;
      GLuint mSize = 0;
      GLuint mRangeOffset = -1;
      GLuint mRangeSize = -1;
      bool mEnableDebug = false;
      
      Buffer(GLuint target = -1, GLuint binding = -1);

      //~Buffer();
      void Free();
      void Init(int size, void* data = 0, GLuint flags = 0);
      void BufferSubData(int offset, int size, void* data);
      void BindBuffer() const;
      void BindBuffer(GLuint binding) const;
      void BindBufferBase() const;
      void BindBufferBase(GLuint binding) const;
      void BindBufferRange() const;
      void BindBufferRange(GLuint binding) const;
      void ClearToInt(int i);                   //clear buffer contents to i
      void ClearToUint(unsigned int u);         //clear buffer contents to u
      void ClearToFloat(float f);               //clear buffer contents to f
      void ClearSubDataToInt(int offset, int size, int i);            //clear buffer contents to i
      void ClearSubDataToUint(int offset, int size, unsigned int u);  //clear buffer contents to u
      void ClearSubDataToFloat(int offset, int size, float f);        //clear buffer contents to f
      void CopyToBufferSubData(Buffer& dest);   //copy buffer contents from this to dest
      void CopyFromBufferSubData(Buffer& src);  //copy buffer contents from src to this
      void GetBufferSubData(void* data);        //copy buffer contents to client memory
      void GetBufferSubData(int offset, int size, void* data);        //copy buffer contents to client memory

      void DebugReadFloat(); //to do: make template
      void DebugReadInt();

};

class BufferArray : public Buffer
{
   public:
      BufferArray(GLuint target = -1);
      void Init(int max_elements, int size, void* data = 0, GLuint flags = 0);

      void GetBufferElementSubData(int index, void* data);   //copy buffer element to client memory
      void BufferElementSubData(int index, void* data);      //copy element to buffer from client memory
      bool PushBack(void* data);

      void SetFirstRangeElement(int n);
      void SetNumRangeElements(int n);

      int mMaxElements = 0;
      int mNumElements = 0;
      int mElementSize = 0;
};
