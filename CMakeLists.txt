cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Incldue CPM
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
set(CPM_VERSION 0.18)

if(NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
  message(STATUS "Downloading CPM.cmake")
  file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif(NOT EXISTS ${CPM_DOWNLOAD_LOCATION})

include(${CPM_DOWNLOAD_LOCATION})

# Start project config
SET (PROJECT_NAME "Just Fast")
SET (PROJECT_DESCRIPTION "Just Fast is a modal file manager, to execute any action you need to select each file and enter a mode (copy, move, delete) once you selected every file and a mode, you can confirm the operation with the confirm key.")

project(JustFast)
add_subdirectory(src)
