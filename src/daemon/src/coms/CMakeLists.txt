set(coms_files
  II2CDevice.h
  Pi4I2CDevice.h
  Pi4I2CDevice.cpp
  WiringPiWrapper.h
  WiringPiWrapper.cpp
)

add_library(coms ${coms_files})

set(coms_includes
  ../
)

#find_library(wiringPi_LIB wiringPi)
#${wiringPi_LIB}
#find_library(i2c_LIB i2c)

target_link_libraries(coms PRIVATE i2c)
target_include_directories(coms PRIVATE ${coms_includes})