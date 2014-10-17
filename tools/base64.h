#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
void base64e(const char *src, char *result, int length);
void base64d(const char *src, char *result, int *length);
#ifdef __cplusplus
}
#endif
