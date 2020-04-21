# Changelog

Current Version: 1.1.1-dirty

## Unreleased

 - Consecutive identical commands are not stored in the history (issue #55)
 - Change message in case of wrong command
 
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
