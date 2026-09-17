# Test executables

add_executable(test_matrix test_matrix.cpp)
target_link_libraries(test_matrix PRIVATE invariant)
add_test(NAME MatrixTests COMMAND test_matrix)
add_executable(test_regressions test_regressions.cpp header_link.cpp)
target_link_libraries(test_regressions PRIVATE invariant)
add_test(NAME RegressionTests COMMAND test_regressions)
