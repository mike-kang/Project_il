_PATH=$(pwd)
LIB_PATH=${_PATH}/..

read -p "Do you use mtrace? (y/n)" ret

if [ "$ret" == "y" ]; then
  MALLOC_TRACE=mtrace.log  LD_LIBRARY_PATH=${LIB_PATH}:${LIB_PATH}/tools/:${LIB_PATH}/inih_r29/ ${_PATH}/main -c FID.test.ini
else
  LD_LIBRARY_PATH=${LIB_PATH}:${LIB_PATH}/tools/:${LIB_PATH}/inih_r29/ ${_PATH}/acu -c FID.test.ini
fi  
