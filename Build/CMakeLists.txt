cmake_minimum_required (VERSION 2.8)
project(trabalho)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

include(cotire)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)
pkg_check_modules(GTK gtk+-3.0)

link_directories(${GTKMM_LIBRARY_DIRS}  ${GTK_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS} ${GTK_INCLUDE_DIRS})

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
#removed from above -Wall -Wextra -Wconversion

FILE(GLOB CPP_FILES ../*.cc)

add_executable(exe ${CPP_FILES})

target_link_libraries(exe ${GTKMM_LIBRARIES}  ${GTK_LIBRARIES})

cotire(trabalho)
