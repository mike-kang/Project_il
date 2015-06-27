#include "memory.h"
#include <malloc.h>

namespace tools {
namespace memory {

int getHeapSize()
{
  struct mallinfo _meminfo = mallinfo();
  
  return _meminfo.hblkhd + _meminfo.uordblks;
}

}
}
