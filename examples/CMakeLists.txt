# Example executables

add_executable(sample_usage sample_usage.cpp)
target_link_libraries(sample_usage PRIVATE invariant)

add_executable(linear_fit linear_fit.cpp)
target_link_libraries(linear_fit PRIVATE invariant)

add_executable(jacobi_interpolation jacobi_interpolation.cpp)
target_link_libraries(jacobi_interpolation PRIVATE invariant)

if(BUILD_TESTS)
    foreach(example sample_usage linear_fit jacobi_interpolation)
        add_test(NAME Example_${example} COMMAND ${example})
    endforeach()
endif()

add_executable(checked_solve checked_solve.cpp)
target_link_libraries(checked_solve PRIVATE invariant::invariant)
if(BUILD_TESTS)
    add_test(NAME Example_checked_solve COMMAND checked_solve)
endif()
