![C/C++ CI of Cli](https://github.com/daniele77/cli/workflows/C/C++%20CI%20of%20Cli/badge.svg)

# cli

A cross-platform header only C++14 library for interactive command line interfaces (Cisco style)

![demo_local_session](https://user-images.githubusercontent.com/5451767/51046611-d1dadc00-15c6-11e9-8a0d-2c66efc83290.gif)

![demo_telnet_session](https://user-images.githubusercontent.com/5451767/51046612-d1dadc00-15c6-11e9-83c2-beadb3593348.gif)

![C/C++ CI of Cli](https://github.com/daniele77/cli/workflows/C/C++%20CI%20of%20Cli/badge.svg)

[:heart: Sponsor](https://github.com/sponsors/daniele77)

**IMPORTANT: Breaking API changes!** Version 2.0 of `cli` has made breaking changes in order to add more functionality.
To migrate your application to new `cli` version, see the section
"Async programming and Schedulers" of this file, or the examples that come with the library.

## Features

* Header only
* Cross-platform (linux and windows)
* Menus and submenus
* Remote sessions (telnet)
* Persistent history (navigation with arrow keys)
* Autocompletion (with TAB key)
* Async interface
* Colors

## How to get CLI library

* From [GitHub](https://github.com/daniele77/cli/releases)
* Using [Vcpkg](https://github.com/Microsoft/vcpkg)

## Dependencies

The library has no dependencies if you don't need remote sessions.

The library depends on asio (either the standalone version or the boost version)
*only* to provide telnet server (i.e., remote sessions).

## Installation

The library is header-only: it consists entirely of header files
containing templates and inline functions, and requires no separately-compiled
library binaries or special treatment when linking.

Extract the archive wherever you want.

Now you must only remember to specify the cli (and optionally asio or boost) paths when
compiling your source code.

If you fancy it, a Cmake script is provided. To install you can use:

    mkdir build && cd build
    cmake ..
    sudo make install

or, if you want to specify the installation path:

    mkdir build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX:PATH=<cli_install_location>
    make install
    
Alternatively, you can use CMake's `FetchContent` module to include CLI library in your project directly.
Add something like this in your `CMakeLists.txt` file:

    include(FetchContent)
    FetchContent_Declare(
      cli
      GIT_REPOSITORY https://github.com/daniele77/cli.git
      GIT_TAG v2.0.2
    )
    FetchContent_MakeAvailable(cli)
    
    add_executable(main-project)
    target_link_libraries(main-project PRIVATE cli::cli)


## Compilation of the examples

You can find some examples in the directory "examples".
Each .cpp file corresponds to an executable.
Each example can be compiled by including cli
(and optionally asio/boost header files) 
and linking pthread on linux (and optionally boost system).

To compile the examples using cmake, use:

    mkdir build && cd build

    # compile only the examples that do not require boost/asio libraries
    cmake .. -DCLI_BuildExamples=ON

    # compile the examples by using boost asio libraries
    cmake .. -DCLI_BuildExamples=ON -DCLI_UseBoostAsio=ON
    # or: cmake .. -DCLI_BuildExamples=ON -DCLI_UseBoostAsio=ON -DBOOST_ROOT=<boost_path>

    # compile the examples by using standalone asio library
    cmake .. -DCLI_BuildExamples=ON -DCLI_UseStandaloneAsio=ON
    # or: cmake .. -DCLI_BuildExamples=ON -DCLI_UseStandaloneAsio=ON -DASIO_INCLUDEDIR=<asio_path>

    cmake --build .

In the same directory you can also find:

* GNU make files (Makefile.noasio, Makefile.boostasio, Makefile.standaloneasio)
* Windows nmake files (makefile.noasio.win, makefile.boostasio.win, makefile.standaloneasio.win)
* a Visual Studio solution

If needed, you can specify asio library path in the following ways:

### GNU Make

for boost:

    make CXXFLAGS="-isystem <boost_include>" LDFLAGS="-L<boost_lib>"

example:

    make CXXFLAGS="-isystem /opt/boost_1_66_0/install/x86/include" LDFLAGS="-L/opt/boost_1_66_0/install/x86/lib"

for standalone asio:

    make CXXFLAGS="-isystem <asio_include>"

example:

    make CXXFLAGS="-isystem /opt/asio-1.18.0/include"

(if you want to use clang instead of gcc, you can set the variable CXX=clang++)

### Windows nmake

Optionally set the environment variable ASIO or BOOST to provide the library path.
Then, from a visual studio console, start `nmake` passing one of the `makefile.*.win` files.

E.g., from a visual studio console, use one of the following commands:

    # only compile examples that do not require asio
    nmake /f makefile.noasio.win
    # compile examples using boost asio
    set BOOST=<path of boost libraries>
    nmake /f makefile.boostasio.win
    # compile examples using standalone asio
    set ASIO=<path of asio library>
    nmake /f makefile.standaloneasio.win

### Visual Studio solution

Currently, the VS solution compiles the examples only with the BOOST dependency.

Set the environment variable BOOST. Then, open the file
`cli/examples/examples.sln`

## Compilation of the Doxygen documentation

If you have doxygen installed on your system, you can get the html documentation
of the library in this way:

    cd doc/doxy
    doxygen Doxyfile

## CLI usage

### Commands and shortcuts

At the start of your  application, cli is presenting a prompt with the
name you provided in the `Cli` constructor.
This means you're in the root menu.

To enter in a submenu, just type the submenu name:
you should see the prompt change to reflect the name of the current submenu.

To go back to the parent menu, you must enter the name of the parent menu.

In any menu, you can enter:
- `exit`: to exit the cli
- `help`: to print the list of the commands available with a description 
- a submenu name: to enter the submenu
- the parent menu name: to return to the the parent menu
- a command in the current menu: to exec the command
- a command in a submenu (using the full path): to exec the command

You can also use:
- up and down arrow keys: to navigate the history of commands
- tab key: to autocomplete the command / menu

### Parameter parsing

The cli interpreter can manage correctly sentences using quote (') and double quote (").
Any character (spaces too) comprises between quotes or double quotes are considered as a single parameter of a command.
The characters ' and " can be used inside a command parameter by escaping them with a backslash.

Some example:

    cli> echo "this is a single parameter"
    this is a single parameter
    cli> echo 'this too is a single parameter'
    this too is a single parameter
    cli> echo "you can use 'single quotes' inside double quoted parameters"
    you can use 'single quotes' inside double quoted parameters
    cli> echo 'you can use "double quotes" inside single quoted parameters'
    you can use "double quotes" inside single quoted parameters
    cli> echo "you can escape \"quotes\" inside a parameter"               
    you can escape "quotes" inside a parameter
    cli> echo 'you can escape \'single quotes\' inside a parameter'
    you can escape 'single quotes' inside a parameter
    cli> echo "you can also show backslash \\ ... "                
    you can also show backslash \ ... 

## Async programming and Schedulers

`cli` is an asynchronous library, and the handlers of commands are executed
by a scheduler, in a thread provided by the user (possibly the main thread),
this allows you to develop a single thread application
without need to worry about synchronization.

So, your application must have a scheduler and pass it to `CliLocalTerminalSession`. 

The library provides three schedulers:

- `LoopScheduler`
- `BoostAsioScheduler`
- `StandaloneAsioScheduler`

`LoopScheduler` is the simplest: it does not depend on other libraries
and should be your first choice if you don't need remote sessions.

`BoostAsioScheduler` and `StandaloneAsioScheduler` are wrappers around
asio `io_context` objects.
You should use one of them if you need a `BoostAsioCliTelnetServer` or a `StandaloneAsioCliTelnetServer`
because they internally use `boost::asio` and `asio`.

You should use one of them also if your application uses `asio` in some way.

After setting up your application, you must call `Scheduler::Run()`
to enter the scheduler loop. Each comamnd handler of the library
will execute in the thread that called `Scheduler::Run()`.

You can exit the scheduler loop by calling `Scheduler::Stop()`
(e.g., as an action associated to the "exit" command).

You can submit work to be executed by the scheduler invoking the method
`Scheduler::Post(const std::function<void()>& f)`.
Schedulers are thread safe, so that you can post function object
from any thread, to be executed in the scheduler thread.

This is an example of use of `LoopScheduler`:

```C++
...
LoopScheduler scheduler;
CliLocalTerminalSession localSession(cli, scheduler);
...
// in another thread you can do:
scheduler.Post([](){ cout << "This will be executed in the scheduler thread" << endl; });
...
// start the scheduler main loop
// it will exit from this method only when scheduler.Stop() is called
// each cli callback will be executed in this thread
scheduler.Run();
...
```

This is an example of use of `BoostAsioScheduler`

```C++
...
BoostAsioScheduler scheduler;
CliLocalTerminalSession localSession(cli, scheduler);
BoostAsioCliTelnetServer server(cli, scheduler, 5000);
...
scheduler.Run();
...
```

Finally, this is an example of use of `BoostAsioScheduler`
when your application already uses `boost::asio` and has
a `boost::asio::io_context` object (the case of standalone asio is similar). 

```C++
...
// somewhere else in your application
boost::asio::io_context ioc;
...
// cli setup
BoostAsioScheduler scheduler(ioc);
CliLocalTerminalSession localSession(cli, scheduler);
BoostAsioCliTelnetServer server(cli, scheduler, 5000);
...
// somewhere else in your application
ioc.run();
...
```

## Adding menus and commands

You must provide at least a root menu for your cli:

```
// create a menu (this is the root menu of our cli)
auto rootMenu = make_unique<Menu>("myprompt");

... // fills rootMenu with commands

// create the cli with the root menu
Cli cli(std::move(rootMenu));

```

You can add menus to existing menus, to get a hierarchy:

```
auto rootMenu = make_unique<Menu>("myprompt");

auto menuA = make_unique<Menu>("a_prompt");
rootMenu->Insert( std::move(menuA) );

auto menuAA = make_unique<Menu>("aa_prompt");
menuA->Insert( std::move(menuAA) );

auto menuAB = make_unique<Menu>("ab_prompt");
menuA->Insert( std::move(menuAB) );

auto menuAC = make_unique<Menu>("ac_prompt");
menuA->Insert( std::move(menuAC) );

auto menuACA = make_unique<Menu>("aca_prompt");
menuAC->Insert( std::move(menuACA) );

auto menuB = make_unique<Menu>("b_prompt");
rootMenu->Insert( std::move(menuB) );

auto menuBA = make_unique<Menu>("ba_prompt");
menuB->Insert( std::move(menuBA) );

auto menuBB = make_unique<Menu>("bb_prompt");
menuB->Insert( std::move(menuBB) );
```

This results in this tree:

```
myprompt
    |
    +--- a_prompt
    |        |
    |        +--- aa_prompt
    |        |
    |        +--- ab_prompt
    |        |
    |        +--- ac_prompt
    |                |
    |                +--- aca_prompt
    |
    +--- b_prompt
             |
             +--- ba_prompt
             |
             +--- bb_prompt

```

Finally, you can add commands to menus, using the `Menu::Insert` method.
The library supports adding commands handler via:
- free functions
- `std::function`
- lambdas

```

static void foo(std::ostream& out, int x) { out << x << std::endl; }

std::function<void(std::ostream& out, int x)> fun(foo);

...

myMenu->Insert("free_function", foo);

myMenu->Insert("std_function", fun);

myMenu->Insert("lambda", [](std::ostream& out, int x){ out << x << std::endl; } );

```

There is no limit to the number of parameters that a command handler can take.
They can be basic types and `std::string`s

```
myMenu->Insert(
    "mycmd", 
    [](std::ostream& out, int a, double b, const std::string& c, bool d, long e)
    { 
        ...
    } );

myMenu->Insert(
    "complex", 
    [](std::ostream& out, std::complex c)
    { 
        ...
    } );
```

or they can be custom types overloading the `std::istream << operator`:

```
struct Foo
{
    friend istream & operator >> (istream &in, Foo& p);
    int value;
};

istream & operator >> (istream& in, Foo& p)
{
    in >> p.value;
    return in;
}

myMenu->Insert(
    "foo", 
    [](std::ostream& out, Foo f)
    { 
        ...
    } );

```

If you need it, you can have a command handlers taking an arbitrary
number of `std::string` parameters:

```
myMenu->Insert(
    "mycmd", 
    [](std::ostream& out, const std::vector<std::string>& pars)
    { 
        ...
    } );

```

Please note that in this case your command handler must take *only one*
parameter of type `std::vector<std::string>`.

## License

Distributed under the Boost Software License, Version 1.0.
(See accompanying file [LICENSE.txt](LICENSE.txt) or copy at
<http://www.boost.org/LICENSE_1_0.txt>)

## Contact

Please report issues here:
<http://github.com/daniele77/cli/issues>

and questions, feature requests, ideas, anything else here:
<http://github.com/daniele77/cli/discussions>

You can always contact me via twitter at @DPallastrelli

---

## Contributing (We Need Your Help!)

Any feedback from users and stakeholders, even simple questions about
how things work or why they were done a certain way, carries value
and can be used to improve the library.

Even if you just have questions, asking them in [GitHub Discussions](http://github.com/daniele77/cli/discussions)
provides valuable information that can be used to improve the library - do not hesitate,
no question is insignificant or unimportant!

If you or your company uses cli library, please consider becoming a sponsor to keep the project strong and dependable.
