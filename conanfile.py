from conans import ConanFile, CMake, tools

class Cli(ConanFile):
    name = "cli"
    version = "v1.2.1"
    license = "BSL-1.0"
    url = "https://github.com/daniele77/cli"
    settings = "os", "compiler", "arch", "build_type"
    options = {"use_boost": [True, False]}
    default_options = {"use_boost": True}
    generators = "cmake"

    def package_id(self):
        if not self.options.use_boost:
            self.info.header_only()

    def source(self):
        tools.get("https://github.com/daniele77/cli/archive/refs/tags/v1.2.1.tar.gz")
        tools.replace_in_file("cli-1.2.1/CMakeLists.txt", "project(cli VERSION 1.2.1 LANGUAGES CXX)",
                              '''project(cli VERSION 1.2.1 LANGUAGES CXX)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()''')

    def requirements(self):
        if self.options.use_boost:
            self.requires("boost/[>1.55.0]")

    def build(self):
        if self.options.use_boost:
            cmake = CMake(self)
            cmake.definitions["CLI_UseBoostAsio"] = "ON"
            # make sure to link against static in this case
            if not self.options["boost"].shared:
                tools.replace_in_file("cli-1.2.1/test/CMakeLists.txt", 'target_compile_definitions(test_suite PRIVATE "BOOST_TEST_DYN_LINK=1")',
                                    '#target_compile_definitions(test_suite PRIVATE "BOOST_TEST_DYN_LINK=1")')
            cmake.definitions["CLI_BuildExamples"] = "ON"
            if tools.get_env("CONAN_RUN_TESTS",True):
                cmake.definitions["CLI_BuildTests"] = "ON"
            cmake.configure(source_folder="cli-1.2.1")
            cmake.build()
            if tools.get_env("CONAN_RUN_TESTS",True):
                cmake.test()

    def package(self):
        self.copy("*.h", src="cli-1.2.1")
        self.copy("*", dst="bin", src="bin", keep_path=False)

    def deploy(self):
        self.copy("*", dst="bin", src="bin")
