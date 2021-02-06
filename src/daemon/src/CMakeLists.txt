add_subdirectory(ahrs)
add_subdirectory(coms)
add_subdirectory(utility)

set(header_files
    sbrcontroller.h
    sensors.h
    ahrs/AHRS.h
    ahrs/AHRSController.h
)

link_directories(${CODE_ROOT}/FancyLogger/bin)

add_executable(sbrcontrollerd main.cpp ${header_files})

set(sbr_includes
./
ahrs
ahrs/algorithms
ahrs/sensors
utility
coms
)

find_package(Threads REQUIRED)

target_include_directories(sbrcontrollerd PRIVATE ${sbr_includes})
target_link_libraries(sbrcontrollerd PRIVATE ahrs coms utility Threads::Threads)
