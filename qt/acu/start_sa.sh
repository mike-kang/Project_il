_PATH=/home/pi/acu
LIB_PATH=${_PATH}/libs/

ulimit -c unlimited

while [ 1 ]
do
  LD_LIBRARY_PATH=${LIB_PATH}:${LIB_PATH}/tools/:${LIB_PATH}/camera/:${LIB_PATH}/inih_r29/ xinit ${_PATH}/acu $* -- :1

  sleep 2

done
