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

#ifndef CLI_DETAIL_LINUXKEYBOARD_H_
#define CLI_DETAIL_LINUXKEYBOARD_H_

#include <thread>
#include <memory>
#include <stdexcept>

#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <cassert>
#include <condition_variable>
#include "inputdevice.h"


namespace cli
{
namespace detail
{

class InputSource
{
public:

    InputSource()
    {
        int pipes[2];
        if (pipe(pipes) == 0)
        {
            shutdownPipe = pipes[1]; // we store the write end
            readPipe = pipes[0]; // ... and the read end
        }
    }

    void WaitKbHit()
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(readPipe, &rfds);

        while (select(readPipe + 1, &rfds, nullptr, nullptr, nullptr) == 0);

        if (FD_ISSET(readPipe, &rfds)) // stop called
        {
            close(readPipe);
            throw std::runtime_error("InputSource stop");
        }

        if (FD_ISSET(STDIN_FILENO, &rfds)) // char from stdinput
        {
            return;
        }

        // cannot reach this point
        assert(false);
    }

    void Stop()
    {
        auto unused = write(shutdownPipe, " ", 1);
        unused = close(shutdownPipe);
        static_cast<void>(unused); // silence unused warn
        shutdownPipe = -1;
    }

private:
    int shutdownPipe;
    int readPipe;
};

//


class LinuxKeyboard : public InputDevice
{
public:
    explicit LinuxKeyboard(Scheduler& _scheduler) :
        InputDevice(_scheduler),
        enabled(false),
        servant( [this]() noexcept { Read(); } )
    {
        ActivateInput();
    }
    ~LinuxKeyboard() override
    {
        ToStandardMode();
        is.Stop();
        servant.join();
    }
    void ActivateInput() override
    {
        ToManualMode();
        std::lock_guard<std::mutex> lock(mtx);
        enabled = true;
        cv.notify_one();
    }
    void DeactivateInput() override
    {
        ToStandardMode();
        std::lock_guard<std::mutex> lock(mtx);
        enabled = false;
    }

private:

    void Read() noexcept
    {
        try
        {
            while (true)
            {
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this]{ return enabled; }); // release mtx, suspend thread execution until enabled becomes true
                }
                auto k = Get();
                Notify(k);
            }
        }
        catch(const std::exception&)
        {
            // nothing to do: just exit
        }
    }

    char GetChar()
    {
        char buffer = 0;
        auto unused = read(0, &buffer, 1);
        static_cast<void>(unused); // silence unused warn
        return buffer;
    }

    std::pair<KeyType,char> Get()
    {
        is.WaitKbHit();

        auto ch = GetChar();
        switch(ch)
        {
            case EOF:
            case 4:  // EOT
                return std::make_pair(KeyType::eof,' ');
                break;
            case 127:
            case 8:
                return std::make_pair(KeyType::backspace,' '); break;
            case 10: return std::make_pair(KeyType::ret,' '); break;
            case 27: // symbol
                ch = GetChar();
                if ( ch == 91 ) // arrow keys
                {
                    ch = GetChar();
                    switch( ch )
                    {
                        case 51:
                            ch = GetChar();
                            if ( ch == 126 ) return std::make_pair(KeyType::canc,' ');
                            else return std::make_pair(KeyType::ignored,' ');
                            break;
                        case 65: return std::make_pair(KeyType::up,' ');
                        case 66: return std::make_pair(KeyType::down,' ');
                        case 68: return std::make_pair(KeyType::left,' ');
                        case 67: return std::make_pair(KeyType::right,' ');
                        case 70: return std::make_pair(KeyType::end,' ');
                        case 72: return std::make_pair(KeyType::home,' ');
                        default: return std::make_pair(KeyType::ignored,' ');
                    }
                }
                break;
            default: // ascii
            {
                const char c = static_cast<char>(ch);
                return std::make_pair(KeyType::ascii,c);
            }
        }
        return std::make_pair(KeyType::ignored,' ');
    }

    void ToManualMode()
    {
        constexpr tcflag_t ICANON_FLAG = ICANON;
        constexpr tcflag_t ECHO_FLAG = ECHO;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~( ICANON_FLAG | ECHO_FLAG );
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    }

    void ToStandardMode()
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }

    bool enabled;
    termios oldt;
    termios newt;
    InputSource is;
    std::thread servant;
    std::mutex mtx;
    std::condition_variable cv;
};

} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_LINUXKEYBOARD_H_

