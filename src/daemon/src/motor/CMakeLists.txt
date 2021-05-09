set(motor_source_files
  IMotorController.h
  ODriveController.h
  ODriveController.cpp
  ISBRController.h
  SBRController.h
  SBRController.cpp
)

add_library(motor ${motor_source_files})
target_link_libraries(motor PRIVATE ahrs fmt::fmt spdlog)

set(motor_includes
../
../ahrs
../coms
../utility
)

target_include_directories(motor PRIVATE ${motor_includes})