# Changelog

Current Version: 1.0-dirty

## Unreleased

- Dynamically Remove/Disable/Enable commands and submenus
- New variadic template method to add commands and menu (makes Add() deprecated)
- Explicitly set the names of parameters in help description

## [1.0.0] - 2019-02-16

- Fix Clang 3.6 - 7.0 undefined reference bug
- Fix infinite loop on EoF
- Fix issue #27 (odd history behaviour): the history now works like in bash
- Fix issue #28 (check for invalid stream in CliFileSession class)
- Add doxygen basic configuration
- Add session recordings in README file
- Add CHANGELOG.md, CODE_OF_CONDUCT.md, CONTRIBUTING.md files
