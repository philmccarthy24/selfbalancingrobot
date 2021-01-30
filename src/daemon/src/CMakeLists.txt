add_subdirectory(ahrs)
#add_subdirectory(coms)
#add_subdirectory(utility)
#add_library(sbrlib) for any sources directly in this src dir

set(header_files
    sbrcontroller.h
    ahrs/AHRS.h
    ahrs/AHRSController.h
)

add_executable(sbrcontrollerd main.cpp ${header_files})
set(sbr_includes
ahrs
ahrs/algorithms
ahrs/sensors
utility
coms
)
target_include_directories(sbrcontrollerd PRIVATE ${sbr_includes})

include(FetchContent)

FetchContent_Declare(json
  #GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

target_link_libraries(sbrcontrollerd PRIVATE nlohmann_json::nlohmann_json ahrs)