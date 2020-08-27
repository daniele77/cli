# Changelog

## [1.2.1] - 2020-08-27

 - With Boost >= 1.74 use TS exectuor by default (issue #79)
 - Standard and custom exception handler for cli commands (issue #74)

## [1.2.0] - 2020-06-27

 - History persistence (issue #39)
 - Escaping and sentence support with quote and double quote
 - Remove boost dependency from cli.h: now, if you just use CliFileSession you don't need boost anymore
 - Support commands with variable number of arguments through `vector<string>`
 - Handle CTRL-D (EOF) on linux
 - Handle CTRL-D, CTRL-Z and CTRL-C on windows
 - Consecutive identical commands are not stored in the history (issue #55)
 - Change message in case of wrong command
 - Fix Backspace from remote terminal (issue #52)
 - Fix duplicate autocompletion (issue #67)
 - Add a namespace and folder "detail" (issue #17)
 
## [1.1.1] - 2019-09-16

 - Specify binding IP address in CliTelnetServer (issue #44)
 - Fix compilation with boost v. 1.70.0 (issue #40)
 - Fix assertion in History::Next() when buffer is empty (issue #47)
 - Fix compilation with older boost versions (< 1.66.0)
 - CMake generates cli.pc configuration file for pkg-config

## [1.1.0] - 2019-04-14

- Dynamically Remove/Disable/Enable commands and submenus (issue #15)
- New variadic template method to add commands and menu (makes Add() deprecated)
- Optionally delimitate string parameters by " (issue #38)
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
