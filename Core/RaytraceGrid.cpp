#include "RaytraceGrid.h"
#include "AttriblessRendering.h"
#include "iq.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void RaytraceGrid2D::Init()
{
   mGrid.SetSize(glm::ivec3(256,256,1));
   mGrid.Init();
   mShader.Init();
   image_map.SetComputeShader(mShader);
   image_map.Init();
}

void RaytraceGrid2D::Compute()
{
   mShader.SetMode(1);
   mGrid = image_map.Compute(mGrid);
}


void RaytraceGrid3D::Init()
{
   mGenShader.Init();

   mVoxels.SetSize(glm::ivec3(32));
   mVoxels.Init();
 
   generator_map.SetComputeShader(mGenShader);
   generator_map.Init();

   mRaytraceShader.Init();

   mImage.SetSize(glm::ivec3(1024, 1024, 1));
   mImage.Init();

   raytrace_map.SetComputeShader(mRaytraceShader);
   raytrace_map.Init();
}

void RaytraceGrid3D::GenerateVoxels()
{
   mGenShader.SetMode(0);
   mVoxels.SetAccess(GL_WRITE_ONLY);
   generator_map.Compute(mVoxels);
}

void RaytraceGrid3D::GenerateImage()
{
   mRaytraceShader.SetMode(0);
   mVoxels.BindImageTexture(1, GL_READ_ONLY);
   mImage.SetAccess(GL_WRITE_ONLY);//TODO this gets changed in compute
   raytrace_map.Compute(mImage); 
}

//////////////////////////////////////////////////////////////////////

void RaytraceOctreeGrid3D::Init()
{
   mGenShader.Init();

   const glm::ivec3 voxel_size(512);
   mVoxels.SetSize(voxel_size);
   mVoxels.SetInternalFormat(GL_RGBA8);
   mVoxels.SetLevelsToMax();
   mVoxels.Init();

   int size = glm::max(voxel_size.x, glm::max(voxel_size.y, voxel_size.z));
   mMcen = glm::translate(glm::vec3(-0.5f)) * glm::scale(glm::vec3(1.0f / size));

   UpdateM();

   generator_map.SetComputeShader(mGenShader);
   generator_map.Init();

   mRaytraceShader.Init();
   mRaytraceFragShader.Init();

   mImage.SetSize(glm::ivec3(mImageSize, 1));
   mImage.Init();

   raytrace_map.SetComputeShader(mRaytraceShader);
   raytrace_map.Init();

   mipmap_reduce.SetComputeShader(mMipmapShader);
   mipmap_reduce.Init();

   mMaterialUbo.mMaterial.ka = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
   mMaterialUbo.mMaterial.kd = glm::vec4(0.9f, 0.8f, 0.7f, 1.0f);
   mMaterialUbo.Init();

   mRaytraceBuffer.Init(sizeof(RaytraceData), &mRaytraceData, GL_DYNAMIC_STORAGE_BIT);

	mM_loc = mRaytraceFragShader.GetUniformLocation("M");
	mM_inv_loc = mRaytraceFragShader.GetUniformLocation("Minv");
	mPV_inv_loc = mRaytraceFragShader.GetUniformLocation("PVinv");
		
}

void RaytraceOctreeGrid3D::UpdateM()
{
	mM = glm::translate(mPos) * glm::rotate(mAngle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(mScale) * mMcen;
	mMinv = glm::inverse(mM);
}

void RaytraceOctreeGrid3D::GenerateVoxels()
{
   const int STATIC_MODE = 0;
   const int ANIMATE_MODE = 1;
   mGenShader.SetMode(STATIC_MODE);
   mVoxels.SetAccess(GL_WRITE_ONLY);
   generator_map.Compute(mVoxels);

   GenerateMipmap();
}

void RaytraceOctreeGrid3D::GenerateMipmap()
{
	const int MAX_MODE = 1;
   mMipmapShader.SetMode(MAX_MODE);
   mipmap_reduce.Compute(mVoxels);
}

void RaytraceOctreeGrid3D::GenerateImage(const glm::mat4& P, const glm::mat4& V)
{
   glm::mat4 PV = P*V;
   glm::mat4 PVinv = glm::inverse(PV);
   int PV_inv_loc = mRaytraceShader.GetUniformLocation("PVinv");
   glProgramUniformMatrix4fv(mRaytraceShader.GetShader(), PV_inv_loc, 1, false, glm::value_ptr(PVinv));

   mMaterialUbo.Bind();
   mRaytraceShader.SetMode(0);

   mVoxels.SetAccess(GL_READ_ONLY);
   mVoxels.SetUnit(1);
   mVoxels.BindTextureUnit();

   mImage.SetAccess(GL_WRITE_ONLY);
   raytrace_map.Compute(mImage);
}

void RaytraceOctreeGrid3D::Draw(const glm::mat4& P, const glm::mat4& V)
{
	glm::mat4 PV = P * V;
	glm::mat4 PVinv = glm::inverse(PV);
	glProgramUniformMatrix4fv(mRaytraceFragShader.GetShaderID(), mPV_inv_loc, 1, false, glm::value_ptr(PVinv));
	glProgramUniformMatrix4fv(mRaytraceFragShader.GetShaderID(), mM_inv_loc, 1, false, glm::value_ptr(mMinv));
	glProgramUniformMatrix4fv(mRaytraceFragShader.GetShaderID(), mM_loc, 1, false, glm::value_ptr(mM));

	mMaterialUbo.Bind();
	mRaytraceFragShader.SetMode(0);

	mVoxels.SetAccess(GL_READ_ONLY);
	mVoxels.SetUnit(1);
	mVoxels.BindTextureUnit();

	mRaytraceFragShader.UseProgram();
	draw_attribless_quad();
}

void RaytraceOctreeGrid3D::SetColor(glm::vec4 color)
{
	const int color_loc = 12;
	glProgramUniform4fv(mGenShader.GetShader(), color_loc, 1, glm::value_ptr(color));
}

void RaytraceOctreeGrid3D::SetBrushXform(const glm::mat3& M)
{
	const int xform_loc = 15;
	glm::mat3 Mvoxel = M*glm::mat3(glm::rotate(mAngle, glm::vec3(0.0f, 1.0f, 0.0f)));
	glProgramUniformMatrix3fv(mGenShader.GetShader(), xform_loc, 1, false, glm::value_ptr(Mvoxel));
}

void RaytraceOctreeGrid3D::SetBrushShape(int shape)
{
	const int shape_loc = 16;
	glProgramUniform1i(mGenShader.GetShader(), shape_loc, shape);
}

void RaytraceOctreeGrid3D::Subtract(int hand, glm::vec3 pos, float r)
{
	glm::vec3 p = mMinv*glm::vec4(pos, 1.0);
	glm::ivec3 pi = glm::floor(p);
	glm::ivec3 ri = glm::ceil(glm::vec3(r, r, r));
	glm::ivec3 min = pi - ri - glm::ivec3(1);
	glm::ivec3 max = pi + ri + glm::ivec3(1);

	glProgramUniform3fv(mGenShader.GetShader(), 10, 1, glm::value_ptr(p));
	glProgramUniform1f(mGenShader.GetShader(), 11, r);
	glProgramUniform3iv(mGenShader.GetShader(), 13, 1, &min.x);
	glProgramUniform1i(mGenShader.GetShader(), 14, hand);

	mVoxels.SetAccess(GL_WRITE_ONLY);
	const int SUB_MODE = 2;
	mGenShader.SetMode(SUB_MODE);
	generator_map.ComputeRect(mVoxels, min, max);

	//generate mipmap
	const int MAX_MODE = 1;
	mMipmapShader.SetMode(MAX_MODE);
	mipmap_reduce.ComputeRect(mVoxels, min, max);
}

void RaytraceOctreeGrid3D::Add(int hand, glm::vec3 pos, float r)
{
	glm::vec3 p = mMinv * glm::vec4(pos, 1.0);
	glm::ivec3 pi = glm::floor(p);
	glm::ivec3 ri = glm::ceil(glm::vec3(r, r, r));
	glm::ivec3 min = pi - ri - glm::ivec3(1);
	glm::ivec3 max = pi + ri + glm::ivec3(1);

	glProgramUniform3fv(mGenShader.GetShader(), 10, 1, glm::value_ptr(p));
	glProgramUniform1f(mGenShader.GetShader(), 11, r);
	glProgramUniform3iv(mGenShader.GetShader(), 13, 1, &min.x);
	glProgramUniform1i(mGenShader.GetShader(), 14, hand);

	mVoxels.SetAccess(GL_WRITE_ONLY);
	const int ADD_MODE = 3;
	mGenShader.SetMode(ADD_MODE);
	generator_map.ComputeRect(mVoxels, min, max);

	//generate mipmap
	const int MAX_MODE = 1;
	mMipmapShader.SetMode(MAX_MODE);
	mipmap_reduce.ComputeRect(mVoxels, min, max);
}

void RaytraceOctreeGrid3D::Paint(int hand, glm::vec3 pos, float r)
{
	glm::vec3 p = mMinv * glm::vec4(pos, 1.0);
	glm::ivec3 pi = glm::floor(p);
	glm::ivec3 ri = glm::ceil(glm::vec3(r, r, r));
	glm::ivec3 min = pi - ri - glm::ivec3(1);
	glm::ivec3 max = pi + ri + glm::ivec3(1);

	glProgramUniform3fv(mGenShader.GetShader(), 10, 1, glm::value_ptr(p));
	glProgramUniform1f(mGenShader.GetShader(), 11, r);
	glProgramUniform3iv(mGenShader.GetShader(), 13, 1, &min.x);
	glProgramUniform1i(mGenShader.GetShader(), 14, hand);

	mVoxels.SetAccess(GL_WRITE_ONLY);
	const int PAINT_MODE = 4;
	mGenShader.SetMode(PAINT_MODE);
	generator_map.ComputeRect(mVoxels, min, max);

	//no mipmap for paint
}

void RaytraceOctreeGrid3D::Add(int hand, glm::vec3 ro, glm::vec3 rd, float r)
{
	glm::ivec3 di = glm::vec3(glm::ceil(2.0f*r)); //brush diameter
	
	glProgramUniform1f(mGenShader.GetShader(), 11, r);
	glProgramUniform1i(mGenShader.GetShader(), 14, hand);
	glProgramUniform1i(mMipmapShader.GetShader(), 14, hand);

	//Voxel-space ray origin
	glm::vec3 ro_vox = mMinv * glm::vec4(ro, 1.0);
	const int ro_loc = 31;
	glProgramUniform3fv(mGenShader.GetShader(), ro_loc, 1, glm::value_ptr(ro_vox));

	//Voxel-space ray direction
	glm::vec3 rd_vox = mMinv * glm::vec4(rd, 0.0);
	const int rd_loc = 32;
	glProgramUniform3fv(mGenShader.GetShader(), rd_loc, 1, glm::value_ptr(rd_vox));

	mRaytraceBuffer.BindBufferBase(0);
	const int RAYCAST_ADD_MODE = 5;
	mGenShader.SetMode(RAYCAST_ADD_MODE);
	mVoxels.SetAccess(GL_WRITE_ONLY);
	generator_map.ComputeRect(mVoxels, glm::ivec3(0), di);

	//generate mipmap
	//TODO: fix this readback
	RaytraceData ray;
	mRaytraceBuffer.GetBufferSubData(&ray);

	if(ray.ray_intersection->w < 0)
	{
		const int MAX_RECT_MODE = 3;
		mMipmapShader.SetMode(MAX_RECT_MODE);
		//mipmap_reduce.ComputeRect(mVoxels, glm::ivec3(0), di);
		mipmap_reduce.ComputeRect(mVoxels, glm::ivec3(ray.dirty_rect_min[hand]), glm::ivec3(ray.dirty_rect_max[hand]));
	}
	else
	{
		//else intersect with bounding box

		//NB: 0.5f*mScale in matrix
		glm::mat4 M = glm::inverse(glm::translate(mPos) * glm::rotate(mAngle, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(0.5f*mScale));

		glm::vec2 t = iq::sBox(ro, rd, M, glm::vec3(1.0f));
		if(t[1]>0.0f)
		{	
			glm::vec3 isect = ro+t[1]*rd;
			Add(hand, isect, r);
		}
	}
}

void RaytraceOctreeGrid3D::Subtract(int hand, glm::vec3 ro, glm::vec3 rd, float r)
{
	glm::ivec3 di = glm::vec3(glm::ceil(2.0f*r)); //brush diameter
	
	glProgramUniform1f(mGenShader.GetShader(), 11, r);
	glProgramUniform1i(mGenShader.GetShader(), 14, hand);
	glProgramUniform1i(mMipmapShader.GetShader(), 14, hand);

	ro = mMinv * glm::vec4(ro, 1.0);
	const int ro_loc = 31;
	glProgramUniform3fv(mGenShader.GetShader(), ro_loc, 1, glm::value_ptr(ro));

	rd = mMinv * glm::vec4(rd, 0.0);
	const int rd_loc = 32;
	glProgramUniform3fv(mGenShader.GetShader(), rd_loc, 1, glm::value_ptr(rd));

	mRaytraceBuffer.BindBufferBase(0);
	const int RAYCAST_SUB_MODE = 6;
	mGenShader.SetMode(RAYCAST_SUB_MODE);
	mVoxels.SetAccess(GL_WRITE_ONLY);
	generator_map.ComputeRect(mVoxels, glm::ivec3(0), di);

	//generate mipmap
	//TODO: fix this readback
	RaytraceData ray;
	mRaytraceBuffer.GetBufferSubData(&ray);

	const int MAX_RECT_MODE = 3;
	mMipmapShader.SetMode(MAX_RECT_MODE);
	//mipmap_reduce.ComputeRect(mVoxels, glm::ivec3(0), di);
	mipmap_reduce.ComputeRect(mVoxels, glm::ivec3(ray.dirty_rect_min[hand]), glm::ivec3(ray.dirty_rect_max[hand]));
}

void RaytraceOctreeGrid3D::Paint(int hand, glm::vec3 ro, glm::vec3 rd, float r)
{	
	glm::ivec3 di = glm::ceil(glm::vec3(2 * r)); //brush diameter

	glProgramUniform1f(mGenShader.GetShader(), 11, r);

	ro = mMinv * glm::vec4(ro, 1.0);
	const int ro_loc = 31;
	glProgramUniform3fv(mGenShader.GetShader(), ro_loc, 1, glm::value_ptr(ro));

	rd = mMinv * glm::vec4(rd, 0.0);
	const int rd_loc = 32;
	glProgramUniform3fv(mGenShader.GetShader(), rd_loc, 1, glm::value_ptr(rd));

	glProgramUniform1i(mGenShader.GetShader(), 14, hand);

	const int RAYCAST_PAINT_MODE = 7;
	mGenShader.SetMode(RAYCAST_PAINT_MODE);
	mVoxels.SetAccess(GL_WRITE_ONLY);
	generator_map.ComputeRect(mVoxels, glm::ivec3(0), di);
}