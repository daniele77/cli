add_executable(complete complete.cpp)

target_link_libraries(complete cli::cli)

if (NOT WIN32)
    add_executable(asyncsession asyncsession.cpp)

    target_link_libraries(asyncsession cli::cli)
endif()

add_executable(filesession filesession.cpp)

target_link_libraries(filesession cli::cli)

add_executable(simplelocalsession simplelocalsession.cpp)

target_link_libraries(simplelocalsession cli::cli)

add_executable(pluginmanager pluginmanager.cpp)

target_link_libraries(pluginmanager cli::cli)
