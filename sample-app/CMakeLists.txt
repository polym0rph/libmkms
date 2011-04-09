################################################################################
# Sample App CMAKE BUILD CONFIG
#
# CMAKE HELP: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
################################################################################

# cmake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project name
PROJECT("sample-app" C CXX)

# set some variables
SET(SAMPLE_APP_EXECUTABLE_NAME "sample-app")

SET(MKMS_INCLUDE_DIR "../lib/include/")
SET(MKMS_LIB_DIR "${CMAKE_SOURCE_DIR}/../lib/build/")

# include dirs
INCLUDE_DIRECTORIES(
  ${PROJECT_BINARY_DIR}
  ${MKMS_INCLUDE_DIR}
  "include")

# src dir
FILE(GLOB
  SAMPLE_APP_SRC_FILES
  src/*.cpp)

# link dirs
LINK_DIRECTORIES(
  ${MKMS_LIB_DIR})

# build executable
ADD_EXECUTABLE( ${SAMPLE_APP_EXECUTABLE_NAME} ${SAMPLE_APP_SRC_FILES})

TARGET_LINK_LIBRARIES(${SAMPLE_APP_EXECUTABLE_NAME} mkms)
