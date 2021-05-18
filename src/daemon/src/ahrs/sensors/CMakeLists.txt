set(ahrs_sensors_source_files
  MPU6050Accel.cpp
  MPU6050Accel.h
  MPU6050Gyro.cpp
  MPU6050Gyro.h
  FXAS21002Gyro.cpp
  FXAS21002Gyro.h
  FXOS8700Accel.cpp
  FXOS8700Accel.h
  FXOS8700Mag.cpp
  FXOS8700Mag.h
  TripleAxisMovingAverageFilter.h
  TripleAxisMovingAverageFilter.cpp
)
add_library(ahrs_sensors ${ahrs_sensors_source_files})
target_include_directories(ahrs_sensors PRIVATE ../ ../../ ../../coms)
target_link_libraries(ahrs_sensors PRIVATE fmt::fmt spdlog)