# Changelog
All notable changes to this project will be documented in this file.

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.2] - 2022-08-18

 - Specify signed for char parameters (issue [#149](https://github.com/daniele77/cli/issues/149))
 - CliSession can call OutStream::UnRegister() when it's already destroyed (issue [#153](https://github.com/daniele77/cli/issues/153))

## [2.0.1] - 2022-04-19

 - Add a non-blocking exec method to schedulers (issue [#127](https://github.com/daniele77/cli/issues/127))
 - Add a Menu::Insert method working with free functions as handler
 - Cli::cout() returns a class derived from std::ostream
 - Fix address sanitizer issue with GenericAsioScheduler dtor
 - Fix teardown problem with linux (issue [#132](https://github.com/daniele77/cli/issues/132))
 - Fix teardown problem with windows
 - The prompt is no more shown after exit
 - Telnet server now works on MobaXTerm

## [2.0.0] - 2021-08-25

 - Remove boost dependency for local only sessions (issue [#83](https://github.com/daniele77/cli/issues/83))
 - Now you can use standalone asio library instead of boost asio for remote sessions (issue [#41](https://github.com/daniele77/cli/issues/41))
 - Fix missing echo after ctrl-v paste of command (issue [#72](https://github.com/daniele77/cli/issues/72))
 - Remove the symbol BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT (issue [#89](https://github.com/daniele77/cli/issues/89))
 - Fix unused parameters warning in release mode (issue [#90](https://github.com/daniele77/cli/issues/90))
 - Cli constructor revision (issue [#75](https://github.com/daniele77/cli/issues/75))
 - Special keys not recognized behaves like backspace in windows (issue [#111](https://github.com/daniele77/cli/issues/111))
 - Fix cmake install dependencies (issue [#117](https://github.com/daniele77/cli/issues/117))

## [1.2.1] - 2020-08-27

 - With Boost >= 1.74 use TS exectuor by default (issue [#79](https://github.com/daniele77/cli/issues/79))
 - Standard and custom exception handler for cli commands (issue [#74](https://github.com/daniele77/cli/issues/74))

## [1.2.0] - 2020-06-27

 - History persistence (issue [#39](https://github.com/daniele77/cli/issues/39))
 - Escaping and sentence support with quote and double quote
 - Remove boost dependency from cli.h: now, if you just use CliFileSession you don't need boost anymore
 - Support commands with variable number of arguments through `vector<string>`
 - Handle CTRL-D (EOF) on linux
 - Handle CTRL-D, CTRL-Z and CTRL-C on windows
 - Consecutive identical commands are not stored in the history (issue [#55](https://github.com/daniele77/cli/issues/55))
 - Change message in case of wrong command
 - Fix Backspace from remote terminal (issue [#52](https://github.com/daniele77/cli/issues/52))
 - Fix duplicate autocompletion (issue [#67](https://github.com/daniele77/cli/issues/67))
 - Add a namespace and folder "detail" (issue [#17](https://github.com/daniele77/cli/issues/17))
 
## [1.1.1] - 2019-09-16

 - Specify binding IP address in CliTelnetServer (issue [#44](https://github.com/daniele77/cli/issues/44))
 - Fix compilation with boost v. 1.70.0 (issue [#40](https://github.com/daniele77/cli/issues/40))
 - Fix assertion in History::Next() when buffer is empty (issue [#47](https://github.com/daniele77/cli/issues/47))
 - Fix compilation with older boost versions (< 1.66.0)
 - CMake generates cli.pc configuration file for pkg-config

## [1.1.0] - 2019-04-14

- Dynamically Remove/Disable/Enable commands and submenus (issue [#15](https://github.com/daniele77/cli/issues/15))
- New variadic template method to add commands and menu (makes Add() deprecated)
- Optionally delimitate string parameters by " (issue [#38](https://github.com/daniele77/cli/issues/38))
- Explicitly set the names of parameters in help description
- Unit tests
- CMake support
- Vcpkg support

## [1.0.0] - 2019-02-16

- Fix Clang 3.6 - 7.0 undefined reference bug
- Fix infinite loop on EoF
- Fix issue #27 (odd history behaviour): the history now works like in bash
- Fix issue #28 (check for invalid stream in CliFileSession class)
- Add doxygen basic configuration
- Add session recordings in README file
- Add CHANGELOG.md, CODE_OF_CONDUCT.md, CONTRIBUTING.md files
