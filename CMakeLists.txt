cmake_minimum_required(VERSION 3.4...3.18)
project(Stream LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ version selection")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Python 3.13 COMPONENTS Interpreter Development REQUIRED)
execute_process(
    COMMAND ${Python_EXECUTABLE} -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(pybind11_DIR ${PYBIND11_CMAKE_DIR})
message(STATUS "Using pybind11 found at: ${pybind11_DIR}")

find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(SerialMuxProt bind/src/bind_wrapper.cpp)

target_include_directories(SerialMuxProt
    PUBLIC
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/bind/include"
)

# Needed for importing the "SerialMuxProt" module from the build directory
set(INIT_PY "${CMAKE_CURRENT_BINARY_DIR}/__init__.py")
if(NOT EXISTS ${INIT_PY})
    file(WRITE ${INIT_PY} "")
endif()

add_custom_command(TARGET SerialMuxProt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE_DIR:SerialMuxProt>/$<TARGET_FILE_NAME:SerialMuxProt>
            ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:SerialMuxProt>
)