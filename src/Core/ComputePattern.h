#pragma once
#include "ComputeShader.h"
#include "Timer.h"
#include "Buffer.h"
#include "ImageTexture.h"

//General base class for compute
class ComputePattern
{
   public:
      virtual void Init();
      void SetComputeShader(ComputeShader& cs);
      ComputeShader* GetComputeShader() { return pShader; }
      void Compute(); //caller binds buffers, ping-pongs and sets grid size, num elements

      void SetWantsCompute(bool compute) {mWantsCompute = compute;}
      void SetSubsteps(int s) { mSubsteps = s; }   
      Timer& GetTimer() {return mTimer;}

   protected:

      ComputeShader* pShader = nullptr;
      DoubleBufferedGpuTimer mTimer;

      bool mWantsCompute = true;          //Set to false to disable Compute
      int mSubsteps = 1;                  //How many time to call Dispatch per call to Compute

      const int kInputOutputBinding = 0;  //for in-place operations
      const int kInputBinding = 0;        //for out-of-place operations  
      const int kOutputBinding = 1;       //for out-of-place operations  
};

//BufferMap can be used for in-place and out-of-place one-to-one operations like Map
// applied to buffers (VBO, SSBO, etc.)
class BufferMap : public ComputePattern
{
   public:

      virtual BufferArray Compute(const BufferArray& inout);         //in-place map
      virtual BufferArray Compute(BufferArray in, BufferArray out);  //out-of-place map 
};

//Convenience typedef for ping-pong buffers of arbitrary size
typedef std::vector<BufferArray> PingPongBufferArray;

class BufferStencil : public ComputePattern
{
   public:

      virtual BufferArray Compute(BufferArray in, BufferArray out);
      virtual void Compute(PingPongBufferArray& buffers);
};

//BufferStencil can be used for out-of-place many-to-one or one-to-many operations like Stencil,
// Scatter, Gather or Reshape/Transpose applied to buffers (VBO, SSBO, etc.)
typedef BufferStencil BufferScatter;
typedef BufferStencil BufferGather;
typedef BufferStencil BufferTranspose;

//Odd-even sort

class BufferOddEvenSort : public ComputePattern
{
public:
   virtual BufferArray Compute(const BufferArray& inout); //sort in-place

   void Init() override;
   Buffer mSorted = Buffer(GL_SHADER_STORAGE_BUFFER);
   int* pSorted = nullptr; //persistent mapped pointer to mSorted Buffer
};


//In-place bitonic sort of buffers
class BufferBitonicSort : public ComputePattern
{
   public:
      virtual BufferArray Compute(const BufferArray& inout); //sort in-place

      enum eAlgorithmVariant : int
      {
         eLocalBitonicMergeSortExample = 0,
         eLocalDisperse = 1,
         eBigFlip = 2,
         eBigDisperse = 3,
      };

   protected:
      void Dispatch(int num_workgroups, int h, int alg);
};

//Scan operations applied to buffers. Operation must be associative (sum, min, max, etc.)

class BufferScan : public ComputePattern
{
   public:
      void Init() override;
      virtual BufferArray Compute(const BufferArray& inout); //scan in-place
   
      enum eMode : int
      {
         eUpsweep = 0,
         eDownsweep = 1
      };

   protected:
      int mStrideLoc = -1;
};

//ImageMap can be used for in-place and out-of-place one-to-one operations like Map
// applied to images

class ImageMap : public ComputePattern
{
   public:

      virtual ImageTexture Compute(const ImageTexture& inout);          //in-place map
      virtual ImageTexture Compute(ImageTexture in, ImageTexture out);  //out-of-place map 
      virtual ImageTexture ComputeRect(const ImageTexture& inout, glm::ivec3 min, glm::ivec3 max);      //in-place map
};

//Convenience typedef for ping-pong images of arbitrary size
typedef std::vector<ImageTexture> PingPongImageTexture;

class ImageStencil : public ComputePattern
{
   public:

      virtual ImageTexture Compute(ImageTexture in, ImageTexture out); 
      virtual void Compute(PingPongImageTexture& images);
};

//ImageStencil can be used for out-of-place many-to-one or one-to-many operations like Stencil,
// Scatter, Gather or Reshape/Transpose applied to images
typedef ImageStencil ImageScatter;
typedef ImageStencil ImageGather;
typedef ImageStencil ImageTranspose;

//Odd-even image sort
class ImageOddEvenSort : public ComputePattern
{
   public:

      virtual ImageTexture Compute(const ImageTexture& inout);          //in-place sort
      virtual ImageTexture ComputeN(const ImageTexture& inout);

      void Init() override;
      Buffer mSorted = Buffer(GL_SHADER_STORAGE_BUFFER);
      int* pSorted = nullptr; //persistent mapped pointer to mSorted Buffer
};

//Pattern for operations which read from images and write to buffers,
// like image histogram

class ImageToBufferPattern : public ComputePattern
{
   public:

      virtual BufferArray Compute(const ImageTexture& in, const BufferArray& out);

};

