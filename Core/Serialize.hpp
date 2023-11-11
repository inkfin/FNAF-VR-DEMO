#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "zip.h"
#include "ImageTexture.h"

namespace glm
{

	template<class Archive> void serialize(Archive& archive, glm::vec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::vec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::vec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::ivec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::ivec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::ivec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::uvec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::uvec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::uvec4& v) { archive(v.x, v.y, v.z, v.w); }
	template<class Archive> void serialize(Archive& archive, glm::dvec2& v) { archive(v.x, v.y); }
	template<class Archive> void serialize(Archive& archive, glm::dvec3& v) { archive(v.x, v.y, v.z); }
	template<class Archive> void serialize(Archive& archive, glm::dvec4& v) { archive(v.x, v.y, v.z, v.w); }

	// glm matrices serialization
	template<class Archive> void serialize(Archive& archive, glm::mat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat2& m) { archive(m[0], m[1]); }
	template<class Archive> void serialize(Archive& archive, glm::mat3& m) { archive(m[0], m[1], m[2]); }
	template<class Archive> void serialize(Archive& archive, glm::mat4& m) { archive(m[0], m[1], m[2], m[3]); }
	template<class Archive> void serialize(Archive& archive, glm::dmat4& m) { archive(m[0], m[1], m[2], m[3]); }

	template<class Archive> void serialize(Archive& archive, glm::quat& q) { archive(q.x, q.y, q.z, q.w); }
	template<class Archive> void serialize(Archive& archive, glm::dquat& q) { archive(q.x, q.y, q.z, q.w); }

}

template<class Archive>
void serialize(Archive& archive, ImageTexture &tex)
{
   archive(tex.mTarget);
   archive(tex.mLevels);
   archive(tex.mLayers);
   archive(tex.mInternalFormat);
   archive(tex.mUnit);
   archive(tex.mAccess);
   archive(tex.mWrap);
   archive(tex.mFilter);
   archive(tex.mSize);
}

template<typename T>
bool ZipVector(zip* z, std::vector<T>& vec, const std::string& name)
{
   if (z == nullptr) return false;
   int n = vec.size();
   if (n <= 0) return false;

   int size = n * sizeof(T); //bytes

   zip_source* source = zip_source_buffer(z, vec.data(), size, 0);
   if (source == nullptr) return false;

   int index = zip_file_add(z, name.c_str(), source, ZIP_FL_OVERWRITE);

   if (index < 0) return false;
   return true;
}

template<typename T>
bool UnzipVector(zip* z, std::vector<T>& vec, const std::string& name)
{
   zip_stat_t stat;
   int err = zip_stat(z, name.c_str(), ZIP_FL_NOCASE, &stat);
   if (err != 0) return false;
   if (stat.size <= 0) return false;

   zip_file_t* child_file = zip_fopen(z, name.c_str(), ZIP_FL_NOCASE);
   if (child_file == nullptr) return false;

   int n = stat.size / sizeof(T);
   vec.resize(n);
   int bytes_read = zip_fread(child_file, vec.data(), stat.size);
   int close_err = zip_fclose(child_file);
   if (bytes_read < 0) return false;
   if (close_err != 0) return false;

   return true;
}

bool ZipBinary(zip* z, std::string& str, const std::string& name);
bool UnzipBinary(zip* z, std::string& str, const std::string& name);
