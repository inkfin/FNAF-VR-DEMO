#pragma once
#include <unordered_map>

//https://stackoverflow.com/questions/54493272/an-efficient-approach-to-use-unordered-map-for-representing-a-symmetric-sparse-m
//https://stackoverflow.com/questions/3842353/more-efficient-sparse-matrix-element-accessor

struct cmp_ivec2 
{
   bool operator()(const glm::ivec2& a, const glm::ivec2& b) const 
   {
      if(a.x==b.x)
      {
         return a.y < b.y;
      }
      return a.x < b.x;
   }
};

template <class T>
class SparseMatrix 
{
   public:
      using index2d = glm::ivec2;

   protected:

      std::map<index2d, T, cmp_ivec2> mMap;

   public:

   T operator() (int i, int j) 
   { 
      index2d p(i,j);
      return mMap[p];
   }

   void SetSym(int i, int j, T val)
   {
      index2d p(i,j);
      mMap[p] = val;
      index2d q(j,i);
      mMap[q] = val;
   }

   void EraseSym(int i, int j)
   {
      index2d p(i,j);
      mMap.erase(p);
      index2d q(j,i);
      mMap.erase(q);
   }

   bool Contains(int i, int j)
   {
      return mMap.contains(index2d(i,j));
   }
      
   std::map<index2d, T>::const_iterator RowBegin(int row)
   {
      return mMap.lower_bound(index2d(row, 0));
   }
   std::map<index2d, T>::const_iterator RowEnd(int row)
   {
      return mMap.lower_bound(index2d(row+1, 0));
   }

   std::vector<T> GetRowContents(int row)
   {
      std::vector<T> contents;
      for (auto it = RowBegin(row); it != RowEnd(row); ++it)
      {
         contents.push_back(it->second);
      }
      return contents;
   }

   std::vector<index2d> GetRowIndices(int row)
   {
      std::vector<index2d> indices;
      for (auto it = RowBegin(row); it != RowEnd(row); ++it)
      {
         indices.push_back(it->first);
      }
      return indices;
   }

   void Clear()
   {
      mMap.clear();
   }

   int Size()
   {
      return mMap.size();
   }
};


struct cmp_ivec3 
{
   bool operator()(const glm::ivec3& a, const glm::ivec3& b) const 
   {
      if(a.x==b.x)
      {
         if(a.y==b.y)
         {
            return a.z < b.z;
         }
         return a.y < b.y;
      }
      return a.x < b.x;
   }
};

template <class T>
class SparseTensor3 
{
   public:
      using index3d = glm::ivec3;

   protected:

      std::map<index3d, T, cmp_ivec3> mMap;

   public:

   auto operator() (int i, int j, int k) 
   { 
      index3d p(i,j,k);
      return mMap[p];
   }

   void SetSym(int i, int j, int k, T val)
   {
      //insert all permutations of indices
      index3d a(i,j,k);
      mMap[a] = val;
      index3d b(i,k,j);
      mMap[b] = val;
      index3d c(j,i,k);
      mMap[c] = val;
      index3d d(j,k,i);
      mMap[d] = val;
      index3d e(k,i,j);
      mMap[e] = val;
      index3d f(k,j,i);
      mMap[f] = val;
   }

   void EraseSym(int i, int j, int k)
   {
      //erase all permutations of indices
      index3d a(i,j,k);
      mMap.erase(a);
      index3d b(i,k,j);
      mMap.erase(b);
      index3d c(j,i,k);
      mMap.erase(c);
      index3d d(j,k,i);
      mMap.erase(d);
      index3d e(k,i,j);
      mMap.erase(e);
      index3d f(k,j,i);
      mMap.erase(f);
   }

   bool Contains(int i, int j, int k)
   {
      return mMap.contains(index3d(i,j,k));
   }
      
   std::map<index3d, T>::const_iterator RowBegin(int row)
   {
      return mMap.lower_bound(index3d(row, 0, 0));
   }
   std::map<index3d, T>::const_iterator RowEnd(int row)
   {
      return mMap.lower_bound(index3d(row+1, 0, 0));
   }

   std::vector<T> GetRowContents(int row)
   {
      std::vector<T> contents;
      for (auto it = RowBegin(row); it != RowEnd(row); ++it)
      {
         contents.push_back(it->second);
      }
      return contents;
   }

   std::vector<index3d> GetRowIndices(int row)
   {
      std::vector<index3d> indices;
      for (auto it = RowBegin(row); it != RowEnd(row); ++it)
      {
         indices.push_back(it->first);
      }
      return indices;
   }


   void Clear()
   {
      mMap.clear();
   }

   int Size()
   {
      return mMap.size();
   }
};

