set(ahrs_sensors_source_files
  MPU6050Accel.cpp
  MPU6050Accel.h
  MPU6050Gyro.cpp
  MPU6050Gyro.h
)
add_library(ahrs_sensors ${ahrs_sensors_source_files})
target_include_directories(ahrs_sensors PRIVATE ../ ../../ ../../coms)