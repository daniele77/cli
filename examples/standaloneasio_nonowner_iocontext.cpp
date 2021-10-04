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

#include <cli/standaloneasioscheduler.h>
#include <cli/cli.h>
#include <cli/clilocalsession.h>

using namespace cli;
using namespace std;

#if ASIO_VERSION < 101200
    using IoContext = asio::io_service;
#else
    using IoContext = asio::io_context;
#endif

class UserInterface
{
public:
    explicit UserInterface(IoContext& iocontext) : scheduler(iocontext)
    {
        auto rootMenu = make_unique< Menu >("cli");
        rootMenu->Insert(
                "hello",
                [](std::ostream& out){ out << "Hello, world\n"; },
                "Print hello world" );
        colorCmd = rootMenu->Insert(
                "color",
                [&](std::ostream& out)
                {
                    out << "Colors ON\n";
                    SetColor();
                    colorCmd.Disable();
                    nocolorCmd.Enable();
                },
                "Enable colors in the cli" );
        nocolorCmd = rootMenu->Insert(
                "nocolor",
                [&](std::ostream& out)
                {
                    out << "Colors OFF\n";
                    SetNoColor();
                    colorCmd.Enable();
                    nocolorCmd.Disable();
                },
                "Disable colors in the cli" );

        cli = make_unique<Cli>(std::move(rootMenu));
        // global exit action
        cli->ExitAction( [](auto& out){ out << "Goodbye and thanks for all the fish.\n"; } );
        // std exception custom handler
        cli->StdExceptionHandler(
            [](std::ostream& out, const std::string& cmd, const std::exception& e)
            {
                out << "Exception caught in cli handler: "
                    << e.what()
                    << " handling command: "
                    << cmd
                    << ".\n";
            }
        );

        localSession = make_unique<CliLocalTerminalSession>(*cli, scheduler, std::cout, 200);
        localSession->ExitAction(
            [this](auto& out) // session exit action
            {
                out << "Closing App...\n";
                scheduler.Stop();
            }
        );
    }
private:
    StandaloneAsioScheduler scheduler;
    unique_ptr<Cli> cli;
    unique_ptr<CliLocalTerminalSession> localSession;
    CmdHandler colorCmd;
    CmdHandler nocolorCmd;
};

int main()
{
    try
    {
        // main application that creates an asio io_context and uses it
        IoContext iocontext;
        asio::steady_timer timer(iocontext, std::chrono::seconds(5));
        timer.async_wait([](const error_code&){ cout << "Timer expired!\n"; });

        // cli setup
        UserInterface interface(iocontext);

        // start the asio io_context
#if ASIO_VERSION < 101200
        asio::io_service::work work(iocontext);
#else
        auto work = asio::make_work_guard(iocontext);
#endif
        iocontext.run();

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
