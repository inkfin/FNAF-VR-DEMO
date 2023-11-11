#include <string>
#include "zip.h"

bool ZipBinary(zip* z, std::string& str, const std::string& name)
{
   int n = str.size();
   if (n <= 0) return false;

   zip_source* source = zip_source_buffer(z, str.data(), str.size(), 0);
   if (source == nullptr) return false;

   int index = zip_file_add(z, name.c_str(), source, ZIP_FL_OVERWRITE);

   if (index < 0) return false;
   return true;
}

bool UnzipBinary(zip* z, std::string& str, const std::string& name)
{
   zip_stat_t stat;
   int err = zip_stat(z, name.c_str(), ZIP_FL_NOCASE, &stat);
   if (err != 0) return false;
   if (stat.size <= 0) return false;

   zip_file_t* child_file = zip_fopen(z, name.c_str(), ZIP_FL_NOCASE);
   if (child_file == nullptr) return false;

   int n = stat.size;
   str.resize(n);
   int bytes_read = zip_fread(child_file, str.data(), stat.size);
   int close_err = zip_fclose(child_file);
   if (bytes_read < 0) return false;
   if (close_err != 0) return false;

   return true;
}
