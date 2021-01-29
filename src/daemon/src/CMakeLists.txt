add_subdirectory(ahrs)
#add_subdirectory(coms)
#add_subdirectory(utility)
#add_library(sbrlib) for any sources directly in this src dir

add_executable(sbrcontrollerd main.cpp)
target_link_libraries(sbrcontrollerd ahrs)