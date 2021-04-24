set(calibtool_sources
Calibration.h
Calibration.cpp
main.cpp
)

add_executable(calibtool ${calibtool_sources})

set(calibtool_includes
./
../src
../src/ahrs
../src/ahrs/algorithms
../src/ahrs/sensors
../src/utility
../src/coms
)

find_package(Threads REQUIRED)

target_include_directories(calibtool PRIVATE ${calibtool_includes})
target_link_libraries(calibtool utility spdlog fmt::fmt Threads::Threads)

# copy default config file
add_custom_command(
        TARGET calibtool POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/sbrconfig.json
                ${CMAKE_CURRENT_BINARY_DIR}/sbrconfig.json)