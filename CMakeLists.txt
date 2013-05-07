cmake_minimum_required(VERSION 2.6)
project(tps)
find_package(EIGEN CONFIG PATHS cmake)

macro(show v)
  message("${v} is ${${v}}")
endmacro()

include_directories(${EIGEN_INCLUDE_DIR})
include_directories(src)

file(GLOB SRCS src/*.c src/*.cc src/*.h)

add_library(tps ${SRCS})
add_dependencies(tps eigen)

#add_subdirectory(app/simon-tps)
add_subdirectory(matlab)
enable_testing()
add_subdirectory(test)

include(InstallRequiredSystemLibraries)
include(CPack)


