include(FetchContent REQUIRED)

#GIT_REPOSITORY https://github.com/nlohmann/json.git

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.8.2)

FetchContent_GetProperties(spdlog)
if(NOT spdlog_POPULATED)
  message(STATUS "Populating spdlog")
  FetchContent_Populate(spdlog)
  # find_package(spdlog REQUIRED)
  set(SPDLOG_BUILD_TESTS OFF CACHE INTERNAL "")
  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR} EXCLUDE_FROM_ALL)
  set(SPDLOG_INCLUDE_DIR ${spdlog_SOURCE_DIR}/include)
endif()

set(utility_files
  IConfigSection.h
  JSONConfigSection.h
  JSONConfigSection.cpp
  Register.h
  Register.cpp
  ISBRFactory.h
  SBRProdFactory.h
  SBRProdFactory.cpp
)

add_library(utility ${utility_files})

set(utility_includes
./
../
../ahrs
../ahrs/sensors
../ahrs/algorithms
../coms
)

target_link_libraries(utility PRIVATE nlohmann_json::nlohmann_json coms ahrs spdlog)
target_include_directories(utility PRIVATE ${utility_includes})