cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(atom-detour)

#-----------------------------------------------------------------------
# CPM configuration
#-----------------------------------------------------------------------

set(CPM_MODULE_NAME atom-detour)
set(CPM_LIB_TARGET_NAME ${CPM_MODULE_NAME})

if ((DEFINED CPM_DIR) AND (DEFINED CPM_UNIQUE_ID) AND (DEFINED CPM_TARGET_NAME))
  set(CPM_LIB_TARGET_NAME ${CPM_TARGET_NAME})
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CPM_DIR})
  include(CPM)
else()
  set(CPM_DIR "${CMAKE_CURRENT_BINARY_DIR}/cpm-packages" CACHE TYPE STRING)
  find_package(Git)
  if(NOT GIT_FOUND)
    message(FATAL_ERROR "CPM requires Git.")
  endif()
  if (NOT EXISTS ${CPM_DIR}/CPM.cmake)
    message(STATUS "Cloning repo (https://github.com/iauns/cpm)")
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" clone https://github.com/iauns/cpm ${CPM_DIR}
      RESULT_VARIABLE error_code
      OUTPUT_QUIET ERROR_QUIET)
    if(error_code)
      message(FATAL_ERROR "CPM failed to get the hash for HEAD")
    endif()
  endif()
  include(${CPM_DIR}/CPM.cmake)
endif()

#------------------------------------------------------------------------------
# CPM Modules
#------------------------------------------------------------------------------

CPM_AddModule("atom-ex"
  GIT_REPOSITORY "https://github.com/darfink/atom-ex.git"
  GIT_TAG "master"
  EXPORT_MODULE TRUE)

CPM_AddModule("atom-memory"
  GIT_REPOSITORY "https://github.com/darfink/atom-memory.git"
  GIT_TAG "master")

CPM_AddModule("cpm-udis86"
  GIT_REPOSITORY "https://github.com/darfink/cpm-udis86.git"
  GIT_TAG "master"
  USE_EXISTING_VER TRUE)

CPM_ForceOnlyOneModuleVersion()
CPM_InitModule(${CPM_MODULE_NAME})

#-----------------------------------------------------------------------
# Setup source
#-----------------------------------------------------------------------

set(FILE_DIR "${CMAKE_CURRENT_LIST_DIR}/atom-detour")
set(ModuleHeaders
  "${FILE_DIR}/DetourBase.hpp"
  "${FILE_DIR}/StaticDetour.hpp"
  "${FILE_DIR}/VirtualDetour.hpp")
set(ModuleSources
  "${FILE_DIR}/src/VirtualDetour.cpp"
  "${FILE_DIR}/src/StaticDetour.cpp")

#-----------------------------------------------------------------------
# Setup library
#-----------------------------------------------------------------------

add_library(${CPM_LIB_TARGET_NAME} STATIC ${ModuleHeaders} ${ModuleSources})
target_link_libraries(${CPM_LIB_TARGET_NAME} ${CPM_LIBRARIES})

set_property(TARGET ${CPM_LIB_TARGET_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${CPM_LIB_TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
