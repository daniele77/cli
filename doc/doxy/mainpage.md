# CLI Library

A cross-platform, header-only C++14 library for building interactive command-line interfaces inspired by Cisco-style consoles.

## Features

- Nested menus and submenus
- Built-in help, history and command navigation
- Tab completion
- Local, file-based and remote (Telnet) sessions
- Asynchronous sessions
- Terminal colors
- Portable Linux, Windows and macOS support
- Optional standalone or Boost.Asio integration

## Getting started

The library contains header-only components under `include/cli`. The central entry points are:

- `cli::Cli` — configures the menu and command-line behavior
- `cli::Menu` — defines menus, submenus and commands
- `cli::CliSession` and `cli::CliLocalSession` — run interactive sessions
- `cli::CliRemoteSession` — provides Telnet-based remote sessions

See the [GitHub repository](https://github.com/daniele77/cli) for installation instructions, examples and source code.
