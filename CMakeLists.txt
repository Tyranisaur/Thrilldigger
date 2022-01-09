cmake_minimum_required(VERSION 3.21)

project(Thrilldigger)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)



add_executable(Thrilldigger
    forms/settingswindow.ui
    forms/simulatorwindow.ui
    forms/solverwindow.ui
    src/main.cpp
    src/settingswindow.cpp
    src/simulatorwindow.cpp
    src/board.cpp
    src/solverwindow.cpp
    src/solver.cpp
    src/partitioniterator.cpp
    src/benchmark.cpp
    src/rupeevalues.cpp
    src/dugtype.cpp
    src/problemparameters.cpp
    src/constraint.cpp
    src/partition.cpp
    src/vector2d.cpp
    application.qrc
    headers/rupeevalues.h
    headers/settingswindow.h
    headers/simulatorwindow.h
    headers/board.h
    headers/solverwindow.h
    headers/solver.h
    headers/dugtype.h
    headers/problemparameters.h
    headers/constraint.h
    headers/partition.h
    headers/partitioniterator.h
    headers/benchmark.h
    headers/vector2d.h)


target_include_directories(Thrilldigger PUBLIC .)

target_link_libraries(Thrilldigger PUBLIC Qt6::Widgets Qt6::Core project_options project_warnings )
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_FLAGS "-Wno-useless-cast")
endif()
