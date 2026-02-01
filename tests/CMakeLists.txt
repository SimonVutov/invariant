# Test executables

add_executable(test_matrix test_matrix.cpp)
target_link_libraries(test_matrix PRIVATE invariant)
add_test(NAME MatrixTests COMMAND test_matrix)
