set(motor_source_files
  IMotorController.h
  ODriveController.h
  ODriveController.cpp
  
)

add_library(motor ${motor_source_files})
target_link_libraries(motor PRIVATE fmt::fmt spdlog)

set(motor_includes
../
../coms
../utility
)

target_include_directories(motor PRIVATE ${motor_includes})