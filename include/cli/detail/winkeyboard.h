/*******************************************************************************
 * CLI - A simple command line interface.
 * Copyright (C) 2016 Daniele Pallastrelli
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

#ifndef CLI_DETAIL_WINKEYBOARD_H_
#define CLI_DETAIL_WINKEYBOARD_H_

#include <functional>
#include <string>
#include <thread>
#include <memory>
#include <atomic>
#include "boostasio.h"

#include <conio.h>

#include "inputdevice.h"

namespace cli
{
namespace detail
{

class WinKeyboard : public InputDevice
{
public:
    explicit WinKeyboard(notboost::BoostExecutor ex) :
        InputDevice(ex)
    {
        servant = std::make_unique<std::thread>([this]() { Read(); });
        servant->detach();
    }
    ~WinKeyboard()
    {
        run = false;
    }

private:
    void Read()
    {
        while (run)
        {
            auto k = Get();
            Notify(k);
        }
    }

    std::pair<KeyType, char> Get()
    {
        int c = _getch();
        switch (c)
        {
            case EOF:
            case 4:  // EOT ie CTRL-D
            case 26: // CTRL-Z
            case 3:  // CTRL-C
                return std::make_pair(KeyType::eof, ' ');
                break;

            case 224: // symbol
            {
                c = _getch();
                switch (c)
                {
                case 72:
                    return std::make_pair(KeyType::up, ' ');
                    break;
                case 80:
                    return std::make_pair(KeyType::down, ' ');
                    break;
                case 75:
                    return std::make_pair(KeyType::left, ' ');
                    break;
                case 77:
                    return std::make_pair(KeyType::right, ' ');
                    break;
                case 71:
                    return std::make_pair(KeyType::home, ' ');
                    break;
                case 79:
                    return std::make_pair(KeyType::end, ' ');
                    break;
                case 83:
                    return std::make_pair(KeyType::canc, ' ');
                    break;
                }
            }
            case 8:
                return std::make_pair(KeyType::backspace, c);
                break;
            case 13:
                return std::make_pair(KeyType::ret, c);
                break;
            default: // hopefully ascii
            {
                const char ch = static_cast<char>(c);
                return std::make_pair(KeyType::ascii, ch);
            }
        }
        return std::make_pair(KeyType::ignored, ' ');
    }

    std::atomic<bool> run{true};
    std::unique_ptr<std::thread> servant;
};

} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_WINKEYBOARD_H_
