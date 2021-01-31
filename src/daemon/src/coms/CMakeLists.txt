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

find_library(wiringPi_LIB wiringPi)

target_link_libraries(coms PRIVATE ${wiringPi_LIB})
target_include_directories(coms PRIVATE ${coms_includes})