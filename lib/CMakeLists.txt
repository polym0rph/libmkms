################################################################################
# libmkms CMAKE BUILD CONFIG
#
# CMAKE HELP: http://www.cmake.org/cmake/help/cmake-2-8-docs.html
################################################################################

# cmake requirements
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# project name
PROJECT("libmkms" C CXX)


# lib specifics
SET(MAC_KEYBOARD_MOUSE_SIMULATOR_LIB_NAME "mkms")

# include
INCLUDE_DIRECTORIES(
  ${PROJECT_BINARY_DIR}
  "include")

# src
FILE(GLOB
  MAC_KEYBOARD_MOUSE_SIMULATOR_SRC_FILES
  src/*.cpp)

SET(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} -framework ApplicationServices")

# build executable
ADD_LIBRARY(
  ${MAC_KEYBOARD_MOUSE_SIMULATOR_LIB_NAME}
  SHARED
  ${MAC_KEYBOARD_MOUSE_SIMULATOR_SRC_FILES})


# Setup doxygen

# Check if doxygen is installed
FIND_PACKAGE(Doxygen)

IF(DOXYGEN_FOUND)
  # Add docs target
  ADD_CUSTOM_TARGET(mkms-docs
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation with Doxygen"
    VERBATIM)
ELSE(DOXYGEN_FOUND)
  MESSAGE(FATAL_ERROR "Doxygen not found.")
ENDIF(DOXYGEN_FOUND)

# cleanup docs folder on "make clean"
SET_PROPERTY(DIRECTORY APPEND PROPERTY
  ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen)
