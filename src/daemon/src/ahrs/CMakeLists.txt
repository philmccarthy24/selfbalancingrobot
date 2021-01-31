set(ahrs_source_files
  AHRS.cpp
  AHRS.h
  AHRSController.cpp
  AHRSController.h
)
add_subdirectory(algorithms)
add_subdirectory(sensors)
add_library(ahrs ${ahrs_source_files})
target_include_directories(ahrs PRIVATE ../)