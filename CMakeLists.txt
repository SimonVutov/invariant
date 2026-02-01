cmake_minimum_required(VERSION 3.15)
project(invariant VERSION 1.0.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library target (header-only for templates)
add_library(invariant INTERFACE)

target_include_directories(invariant INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Optional: Build examples
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Optional: Build tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install rules
install(TARGETS invariant
    EXPORT invariantTargets
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT invariantTargets
    FILE invariantTargets.cmake
    NAMESPACE invariant::
    DESTINATION lib/cmake/invariant
)