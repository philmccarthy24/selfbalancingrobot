add_subdirectory(ahrs)
add_subdirectory(coms)
add_subdirectory(utility)

set(header_files
    sbrcontroller.h
    sensors.h
    ahrs/AHRS.h
    ahrs/AHRSController.h
)

add_executable(sbrcontrollerd main.cpp ${header_files})

set(sbr_includes
./
ahrs
ahrs/algorithms
ahrs/sensors
utility
coms
)

target_include_directories(sbrcontrollerd PRIVATE ${sbr_includes})
target_link_libraries(sbrcontrollerd PRIVATE ahrs coms utility)
