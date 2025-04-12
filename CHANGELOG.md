# Changelog
All notable changes to this project will be documented in this file.

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased

 - Fix mutex initialization issue in cli library (pr [#249](https://github.com/daniele77/cli/pull/249))

## [2.2.0] - 2024-10-25

 - Add clear screen command using ctrl + L  (issue [#229](https://github.com/daniele77/cli/issues/229))
 - Add history view and execute an history command with bang + id (issue [#235](https://github.com/daniele77/cli/issues/235))
 - Custom handler for wrong commands (issue [#223](https://github.com/daniele77/cli/issues/223))
 - Parent menus does not chain (issue [#234](https://github.com/daniele77/cli/issues/234))
 - Parent menu shortcut (issue [#233](https://github.com/daniele77/cli/issues/233))
 - Integer overflow warning detected by clang++ (issue [#236](https://github.com/daniele77/cli/issues/236))
 - Enable Keyboard Handling in Command Handlers on Linux Platform (issue [#239](https://github.com/daniele77/cli/issues/239))
 - Fix issue with remote session on ARM platforms (issue [#220](https://github.com/daniele77/cli/issues/220))

## [2.1.0] - 2023-06-29

 - Nest namespace rang (issue [#167](https://github.com/daniele77/cli/issues/167))
 - Add ascii value 8 for backspace (issue [#124](https://github.com/daniele77/cli/issues/124))
 - Add check for CMAKE_SKIP_INSTALL_RULES (issue [#160](https://github.com/daniele77/cli/issues/160))
 - Add enter action (issue [#177](https://github.com/daniele77/cli/issues/177) - PR [#180](https://github.com/daniele77/cli/pull/177)) 
 - Fix missing echo after paste of command (issue [#185](https://github.com/daniele77/cli/issues/185))
 - Fix asio::io_context::work has no member named reset in old asio lib (issue [#188](https://github.com/daniele77/cli/issues/188))
 - Shown in the `complete` and `pluginmanager` examples that the issue #96 was already fixed (issue [#96](https://github.com/daniele77/cli/issues/96))
 - Allow custom prompt for menu (issue [#101](https://github.com/daniele77/cli/issues/101))

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

 - With Boost >= 1.74 use TS executor by default (issue [#79](https://github.com/daniele77/cli/issues/79))
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
