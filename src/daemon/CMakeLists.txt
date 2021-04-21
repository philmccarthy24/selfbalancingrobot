# CMakeList.txt : CMake project for daemon, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)

project ("sbrcontroller")

# resources: https://stackoverflow.com/questions/48509911/cmake-add-subdirectory-vs-include

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(calibtool)
