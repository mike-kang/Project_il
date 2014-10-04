#ifndef _FILESYSTEM_HEADER
#define _FILESYSTEM_HEADER

namespace tools {
namespace filesystem {

bool file_exist(const char*);
int file_size(const char*);
void dir_create(const char*);
char* dir_current();

}
}
#endif //_FILESYSTEM_HEADER
