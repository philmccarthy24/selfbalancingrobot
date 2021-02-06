include(FetchContent)

#GIT_REPOSITORY https://github.com/nlohmann/json.git

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(utility_files
  IConfigProvider.h
  JSONConfigProvider.h
  JSONConfigProvider.cpp
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

target_link_libraries(utility PRIVATE nlohmann_json::nlohmann_json coms ahrs)
target_include_directories(utility PRIVATE ${utility_includes})