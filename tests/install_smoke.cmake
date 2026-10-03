# Copy the installation before consumption to verify relocatability.
function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Command failed (${result}): ${ARGV}")
    endif()
endfunction()
set(prefix "${BINARY_DIR}/install-smoke/staging")
set(relocated "${BINARY_DIR}/install-smoke/relocated")
file(REMOVE_RECURSE "${BINARY_DIR}/install-smoke")
run("${CMAKE_COMMAND}" --install "${BINARY_DIR}" --prefix "${prefix}" --config "${CONFIG}")
file(RENAME "${prefix}" "${relocated}")
run("${CMAKE_COMMAND}" -S "${SOURCE_DIR}/tests/consumer"
    -B "${BINARY_DIR}/install-smoke/consumer" -G "${GENERATOR}"
    "-DCMAKE_CXX_COMPILER=${COMPILER}" "-DCMAKE_PREFIX_PATH=${relocated}")
run("${CMAKE_COMMAND}" --build "${BINARY_DIR}/install-smoke/consumer" --config "${CONFIG}")
run("${CMAKE_CTEST_COMMAND}" --test-dir "${BINARY_DIR}/install-smoke/consumer"
    -C "${CONFIG}" --output-on-failure)
