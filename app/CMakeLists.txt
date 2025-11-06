cmake_minimum_required(VERSION 3.10)
project(clash_bypass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the native library
add_library(clash_bypass SHARED
    src/main/cpp/clash_bypass.cpp
)

# Link against Android log library
target_link_libraries(clash_bypass
    log
    dl
)

# Set output directory
set_target_properties(clash_bypass PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}"
)
