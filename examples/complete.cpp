/*******************************************************************************
 * CLI - A simple command line interface.
 * Copyright (C) 2016-2021 Daniele Pallastrelli
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#ifdef CLI_EXAMPLES_USE_STANDALONEASIO_SCHEDULER
    #include <cli/standaloneasioscheduler.h>
    #include <cli/standaloneasioremotecli.h>
    namespace cli
    {    
        using MainScheduler = StandaloneAsioScheduler;
        using CliTelnetServer = StandaloneAsioCliTelnetServer;
    } // namespace cli
#elif defined(CLI_EXAMPLES_USE_BOOSTASIO_SCHEDULER)
    // TODO: NB boostasioscheduler.h includes boost asio
    // so in Windows it should appear before cli.h and clilocalsession.h that include rang,
    // because both include WinSock.h
    // (consider to provide a global header file for the library)
    #include <cli/boostasioscheduler.h>
    #include <cli/boostasioremotecli.h>
    namespace cli
    {    
        using MainScheduler = BoostAsioScheduler;
        using CliTelnetServer = BoostAsioCliTelnetServer;
    }
#else
    #error either CLI_EXAMPLES_USE_STANDALONEASIO_SCHEDULER or CLI_EXAMPLES_USE_BOOSTASIO_SCHEDULER must be defined
#endif

#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/filehistorystorage.h>

#include <vector>
#include <algorithm> // std::copy

using namespace cli;
using namespace std;

int main()
{
    try
    {
        CmdHandler colorCmd;
        CmdHandler nocolorCmd;
        CmdHandler enableCmd;
        CmdHandler disableCmd;

        // setup cli

        auto rootMenu = make_unique<Menu>("cli");
        rootMenu->Insert(
                "hello",
                [](UserSession& s){ s.out << "Hello, world\n"; },
                "Print hello world" );
        rootMenu->Insert(
                "hello_everysession",
                [](UserSession&){ Cli::cout() << "Hello, everybody" << std::endl; },
                "Print hello everybody on all open sessions" );
        rootMenu->Insert(
                "answer",
                [](UserSession& s, int x){ s.out << "The answer is: " << x << "\n"; },
                "Print the answer to Life, the Universe and Everything" );
        rootMenu->Insert(
                "file",
                [](UserSession& s, int fd)
                {
                    s.out << "file descriptor: " << fd << "\n";
                },
                "Print the file descriptor specified",
                {"file_descriptor"} );
        rootMenu->Insert(
                "echo", {"string to echo"},
                [](UserSession& s, const string& arg)
                {
                    s.out << arg << "\n";
                },
                "Print the string passed as parameter" );
        rootMenu->Insert(
                "echo", {"first string to echo", "second string to echo"},
                [](UserSession& s, const string& arg1, const string& arg2)
                {
                    s.out << arg1 << ' ' << arg2 << "\n";
                },
                "Print the strings passed as parameter" );
        rootMenu->Insert(
                "error",
                [](UserSession&)
                {
                    throw std::logic_error("Error in cmd");
                },
                "Throw an exception in the command handler" );
        rootMenu->Insert(
                "reverse", {"string_to_revert"},
                [](UserSession& s, const string& arg)
                {
                    string copy(arg);
                    std::reverse(copy.begin(), copy.end());
                    s.out << copy << "\n";
                },
                "Print the reverse string" );
        rootMenu->Insert(
                "add", {"first_term", "second_term"},
                [](UserSession& s, int x, int y)
                {
                    s.out << x << " + " << y << " = " << (x+y) << "\n";
                },
                "Print the sum of the two numbers" );
        rootMenu->Insert(
                "add",
                [](UserSession& s, int x, int y, int z)
                {
                    s.out << x << " + " << y << " + " << z << " = " << (x+y+z) << "\n";
                },
                "Print the sum of the three numbers" );
        rootMenu->Insert(
                "sort", {"list of strings separated by space"},
                [](UserSession& s, std::vector<std::string> data)
                {
                    std::sort(data.begin(), data.end());
                    s.out << "sorted list: ";
                    std::copy(data.begin(), data.end(), std::ostream_iterator<std::string>(s.out, " "));
                    s.out << "\n";
                },
                "Alphabetically sort a list of words" );
        colorCmd = rootMenu->Insert(
                "color",
                [&](UserSession& s)
                {
                    if (s["color"] == "yes")
                        return;

                    s["color"] = "yes";
                    s.out << "Colors ON\n";
                    s.terminalProfile.SetColor();
                    //colorCmd.Disable();
                    //nocolorCmd.Enable();
                },
                "Enable colors in the cli" );
        nocolorCmd = rootMenu->Insert(
                "nocolor",
                [&](UserSession& s)
                {
                    if (s["color"] == "no")
                        return;

                    s["color"] = "no";

                    s.out << "Colors OFF\n";
                    s.terminalProfile.SetNoColor();
                    //colorCmd.Enable();
                    //nocolorCmd.Disable();
                },
                "Disable colors in the cli" );
        auto fakeCmd = rootMenu->Insert(
                "fake",
                [&](UserSession& s)
                {
                    s.out << "Done!" << std::endl;
                },
                "Prints a nice message" );
        rootMenu->Insert(
                "removecmd",
                [&](UserSession&)
                {
                    fakeCmd.Remove();
                    disableCmd.Disable();
                    enableCmd.Disable();
                },
                "Remove the fake command from the menu" );
        disableCmd = rootMenu->Insert(
                "disablecmd",
                [&](UserSession&)
                {
                    fakeCmd.Disable();
                    disableCmd.Disable();
                    enableCmd.Enable();
                },
                "Disable the fake command" );
        enableCmd = rootMenu->Insert(
                "enablecmd",
                [&](UserSession&)
                {
                    fakeCmd.Enable();
                    disableCmd.Enable();
                    enableCmd.Disable();
                },
                "Enable the fake command" );

        auto subMenu = make_unique<Menu>("sub");
        subMenu->Insert(
                "hello",
                [](UserSession& s){ s.out << "Hello, submenu world\n"; },
                "Print hello world in the submenu" );
        subMenu->Insert(
                "demo",
                [](UserSession& s){ s.out << "This is a sample!\n"; },
                "Print a demo string" );

        auto subSubMenu = make_unique<Menu>("subsub");
            subSubMenu->Insert(
                "hello",
                [](UserSession& s){ s.out << "Hello, subsubmenu world\n"; },
                "Print hello world in the sub-submenu" );
        subMenu->Insert( std::move(subSubMenu) );

        rootMenu->Insert( std::move(subMenu) );

        // create a cli with the given root menu and a persistent storage
        // you must pass to FileHistoryStorage the path of the history file
        // if you don't pass the second argument, the cli will use a VolatileHistoryStorage object that keeps in memory
        // the history of all the sessions, until the cli is shut down.
        Cli cli( std::move(rootMenu), std::make_unique<FileHistoryStorage>(".cli") );
        // global exit action
        cli.ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );
        // std exception custom handler
        cli.StdExceptionHandler(
            [](std::ostream& out, const std::string& cmd, const std::exception& e)
            {
                out << "Exception caught in cli handler: "
                    << e.what()
                    << " handling command: "
                    << cmd
                    << ".\n";
            }
        );

        MainScheduler scheduler;
        CliLocalTerminalSession localSession(cli, scheduler, std::cout, 200);
        localSession.ExitAction(
            [&scheduler](auto& out) // session exit action
            {
                out << "Closing App...\n";
                scheduler.Stop();
            }
        );

        // setup server

        CliTelnetServer server(cli, scheduler, 5000);
        // exit action for all the connections
        server.ExitAction( [](auto& out) { out << "Terminating this session...\n"; } );

        scheduler.Run();

        return 0;

    }
    catch (const std::exception& e)
    {
        cerr << "Exception caugth in main: " << e.what() << endl;
    }
    catch (...)
    {
        cerr << "Unknown exception caugth in main." << endl;
    }
    return -1;
}
