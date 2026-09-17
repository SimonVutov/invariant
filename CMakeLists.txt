cmake_minimum_required(VERSION 3.20)
project(invariant VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(invariant INTERFACE)
add_library(invariant::invariant ALIAS invariant)
target_compile_features(invariant INTERFACE cxx_std_17)
target_include_directories(invariant INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Keep dependency builds quiet; preserve the original option names.
set(invariant_is_top_level OFF)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(invariant_is_top_level ON)
endif()
option(BUILD_EXAMPLES "Build Invariant examples" ${invariant_is_top_level})
option(BUILD_TESTS "Build Invariant tests" ${invariant_is_top_level})
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

set(invariant_cmake_dir "${CMAKE_INSTALL_LIBDIR}/cmake/invariant")
configure_package_config_file(cmake/invariantConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/invariantConfig.cmake"
    INSTALL_DESTINATION "${invariant_cmake_dir}")
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/invariantConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(TARGETS invariant EXPORT invariantTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT invariantTargets FILE invariantTargets.cmake
    NAMESPACE invariant:: DESTINATION "${invariant_cmake_dir}")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/invariantConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/invariantConfigVersion.cmake"
    DESTINATION "${invariant_cmake_dir}")
install(FILES LICENSE DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/invariant")
