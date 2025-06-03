cmake_minimum_required(VERSION 3.10)
project(SimlanCompiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable
add_executable(simlanc
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
)

# target_include_directories(simlanc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Enable warnings (optional but recommended)
if(MSVC)
    target_compile_options(simlanc PRIVATE /W4)
else()
    target_compile_options(simlanc PRIVATE -Wall -Wextra -pedantic)
endif()

# For easier debugging
set(CMAKE_BUILD_TYPE Debug)

# Output directory for the executable (optional)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

install(TARGETS simlanc DESTINATION bin)

# Simple test (requires demo.simlan to be in the build directory or accessible path)
# This is a very basic test, consider using CTest for more complex testing.
# add_custom_target(run_demo
#     COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/simlanc ${CMAKE_SOURCE_DIR}/demo.simlan
#     DEPENDS simlanc
#     COMMENT "Running simlanc with demo.simlan"
# )
