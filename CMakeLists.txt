cmake_minimum_required(VERSION 3.8)

project(cli VERSION 1.0 LANGUAGES CXX)

option(CLI_BuildExamples "Build the examples." OFF)

find_package(Boost 1.55 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

# Add Library
add_library(cli INTERFACE)
# Add target alias
add_library(cli::cli ALIAS cli)

target_include_directories(cli
        INTERFACE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(cli INTERFACE Boost::system Threads::Threads)
target_compile_features(cli INTERFACE cxx_std_14)

# Examples
if (CLI_BuildExamples)
    add_subdirectory(examples)
endif()

# Install
install(DIRECTORY include DESTINATION .)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"cliConfig.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cliConfig.cmake"
	INSTALL_DESTINATION "lib/cmake/cli"
)

install(TARGETS cli EXPORT cliTargets DESTINATION lib)
install(EXPORT cliTargets FILE cliTargets.cmake NAMESPACE cli:: DESTINATION lib/cmake/cli)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/cliConfig.cmake"
	DESTINATION "lib/cmake/cli"
)
