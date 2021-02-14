set(coms_files
  II2CDevice.h
  LinuxI2CDevice.h
  LinuxI2CDevice.cpp
  WiringPiWrapper.h
  WiringPiWrapper.cpp
  ISerialDevice.h
  LinuxSerialDevice.h
  LinuxSerialDevice.cpp
)

add_library(coms ${coms_files})

set(coms_includes
  ../
)

# Put back in and remove line below if need to move back to wrappers
# find_library(wiringPi_LIB wiringPi)
# target_link_libraries(coms PRIVATE ${wiringPi_LIB})

target_link_libraries(coms PRIVATE i2c fmt::fmt)
target_include_directories(coms PRIVATE ${coms_includes})