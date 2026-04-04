cmake_minimum_required(VERSION 3.5.0)
project(rpi-cam-test VERSION 1.1.0)

include(CTest)
enable_testing()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES main.cpp src/RPiCam.cpp)


find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)


include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(rpi-cam-test ${SOURCES})

target_link_libraries(rpi-cam-test ${OpenCV_LIBS} PkgConfig::LIBCAMERA)
target_include_directories(rpi-cam-test PRIVATE ${INCLUDE_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

