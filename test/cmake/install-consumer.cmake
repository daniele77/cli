# Run with: cmake -DTEST_BINARY_DIR=<scratch directory> -P test/cmake/install-consumer.cmake
cmake_minimum_required(VERSION 3.8...3.27)
get_filename_component(CLI_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)

function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Command failed: ${ARGV}")
    endif()
endfunction()

foreach(mode IN ITEMS compiled none)
    set(root "${TEST_BINARY_DIR}/${mode}")
    set(options -DCLI_UseBoostAsio=ON)
    if(mode STREQUAL "none")
        set(options -DCLI_UseBoostAsio=OFF)
    endif()
    run("${CMAKE_COMMAND}" "-H${CLI_SOURCE_DIR}" "-B${root}/library"
        "-DCMAKE_INSTALL_PREFIX=${root}/prefix" ${options})
    run("${CMAKE_COMMAND}" --build "${root}/library" --target install --config Release)
    run("${CMAKE_COMMAND}" "-H${CMAKE_CURRENT_LIST_DIR}/consumer" "-B${root}/consumer"
        "-DCMAKE_PREFIX_PATH=${root}/prefix")
    run("${CMAKE_COMMAND}" --build "${root}/consumer" --config Release)
endforeach()
