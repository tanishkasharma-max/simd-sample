cmake_minimum_required(VERSION 3.15)
project(wasm_xsimd_brightness LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "Run with: emcmake cmake ..")
endif()

include(FetchContent)
FetchContent_Declare(
    xsimd
    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
    GIT_TAG master
)
FetchContent_MakeAvailable(xsimd)

add_executable(brightness
    src/brightness.cpp
)

target_include_directories(brightness PRIVATE
    ${xsimd_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Compile flags
target_compile_options(brightness PRIVATE
    -O3
    -msimd128
)

# Output name
set_target_properties(brightness PROPERTIES
    OUTPUT_NAME "brightness"
    SUFFIX ".js"
)

# ---- FIXED: Use target_link_options instead of LINK_FLAGS ----
target_link_options(brightness PRIVATE
    "-sWASM=1"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=createBrightnessModule"
    "-sENVIRONMENT=web"
    "-sEXPORT_ES6=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sEXPORTED_FUNCTIONS=[\"_malloc\",\"_free\"]"
    "-sEXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"HEAPU8\"]"
)

