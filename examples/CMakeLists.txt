# Example executables

add_executable(sample_usage sample_usage.cpp)
target_link_libraries(sample_usage PRIVATE invariant)

add_executable(linear_fit linear_fit.cpp)
target_link_libraries(linear_fit PRIVATE invariant)
