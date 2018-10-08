#include "misc.hpp"
#include <iostream>

using namespace std;

bool file_exists (string fp) {
  struct stat buf;   
  bool ret = (stat(fp.c_str(), &buf) == 0);
  if (ret)
  {
      cerr << "Found " << fp << endl;
  } 
  return ret;
}