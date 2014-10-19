#ifndef _UTILS_HEADER
#define _UTILS_HEADER

namespace tools {
namespace utils {
char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}  


}
}
#endif //_UTILS_HEADER