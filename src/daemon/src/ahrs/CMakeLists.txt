set(ahrs_source_files
  AHRS.cpp
  AHRS.h
  AHRSManager.cpp
  AHRSManager.h
)
add_subdirectory(algorithms)
add_subdirectory(sensors)
add_library(ahrs ${ahrs_source_files})
target_link_libraries(ahrs PRIVATE ahrs_algorithms ahrs_sensors utility spdlog)

set(ahrs_includes
algorithms
../
../utility
)

target_include_directories(ahrs PRIVATE ${ahrs_includes})