#ifdef __cplusplus
extern "C" {
#endif

int init_raspistill(int width, int height, char* buf);
int close_raspistill();
int takePicture();


#ifdef __cplusplus
};
#endif
