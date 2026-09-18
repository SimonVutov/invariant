# Test executables

add_executable(test_matrix test_matrix.cpp)
target_link_libraries(test_matrix PRIVATE invariant)
add_test(NAME MatrixTests COMMAND test_matrix)
add_executable(test_regressions test_regressions.cpp header_link.cpp)
target_link_libraries(test_regressions PRIVATE invariant)
add_test(NAME RegressionTests COMMAND test_regressions)
add_test(NAME InstallSmoke COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR=${PROJECT_SOURCE_DIR}
    -DBINARY_DIR=${PROJECT_BINARY_DIR}
    -DGENERATOR=${CMAKE_GENERATOR}
    -DCOMPILER=${CMAKE_CXX_COMPILER}
    -DCONFIG=$<CONFIG>
    -P ${CMAKE_CURRENT_SOURCE_DIR}/install_smoke.cmake)
add_executable(test_least_squares test_least_squares.cpp)
target_link_libraries(test_least_squares PRIVATE invariant)
add_test(NAME LeastSquaresTests COMMAND test_least_squares)
