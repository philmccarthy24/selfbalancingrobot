set(ahrs_algorithms_source_files
  Madgwick.cpp
  Madgwick.h
)
add_library(ahrs_algorithms ${ahrs_algorithms_source_files})
target_include_directories(ahrs_algorithms PRIVATE ../ ../../)