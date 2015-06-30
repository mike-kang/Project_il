LIB_PATH=/home/pi/Project_il

LD_LIBRARY_PATH=${LIB_PATH}:${LIB_PATH}/tools/:${LIB_PATH}/inih_r29/:${LIB_PATH}/camera gdb -c core ./main
