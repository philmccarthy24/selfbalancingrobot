add_subdirectory(ahrs)
add_subdirectory(coms)
add_subdirectory(utility)
add_subdirectory(motor)

set(header_files
    sbrcontroller.h
    sensors.h
    ahrs/AHRS.h
    ahrs/AHRSManager.h
)

add_executable(sbrcontrollerd main.cpp ${header_files})

set(sbr_includes
./
ahrs
ahrs/algorithms
ahrs/sensors
utility
coms
motor
)

find_package(Threads REQUIRED)

target_include_directories(sbrcontrollerd PRIVATE ${sbr_includes})
target_link_libraries(sbrcontrollerd PRIVATE ahrs coms utility motor Threads::Threads spdlog)

# copy default config file
add_custom_command(
        TARGET sbrcontrollerd POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/sbrconfig.json
                ${CMAKE_CURRENT_BINARY_DIR}/sbrconfig.json)
