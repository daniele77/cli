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

#ifndef CLI_DETAIL_TERMINAL_H_
#define CLI_DETAIL_TERMINAL_H_

#include <string>
#include "../colorprofile.h"
#include "inputdevice.h"

namespace cli
{
namespace detail
{

enum class Symbol
{
    nothing,
    command,
    up,
    down,
    tab,
    eof
};

class Terminal
{
  public:
    Terminal(std::ostream& _out, TerminalProfile& _profile) : out(_out), profile(_profile) {}

    void ResetCursor() { position = 0; }

    void SetLine(const std::string &newLine)
    {
        out << profile.beforeInput
            << std::string(position, '\b') << newLine
            << profile.afterInput << std::flush;

        // if newLine is shorter then currentLine, we have
        // to clear the rest of the string
        if (newLine.size() < currentLine.size())
        {
            out << std::string(currentLine.size() - newLine.size(), ' ');
            // and go back
            out << std::string(currentLine.size() - newLine.size(), '\b') << std::flush;
        }

        currentLine = newLine;
        position = currentLine.size();
    }

    std::string GetLine() const { return currentLine; }

    std::pair<Symbol, std::string> Keypressed(std::pair<KeyType, char> k)
    {
        switch (k.first)
        {
            case KeyType::eof:
                return std::make_pair(Symbol::eof, std::string{});
                break;
            case KeyType::backspace:
            {
                if (position == 0)
                    break;

                --position;

                const auto pos = static_cast<std::string::difference_type>(position);
                // remove the char from buffer
                currentLine.erase(currentLine.begin() + pos);
                // go back to the previous char
                out << '\b';
                // output the rest of the line
                out << std::string(currentLine.begin() + pos, currentLine.end());
                // remove last char
                out << ' ';
                // go back to the original position
                out << std::string(currentLine.size() - position + 1, '\b') << std::flush;
                break;
            }
            case KeyType::up:
                return std::make_pair(Symbol::up, std::string{});
                break;
            case KeyType::down:
                return std::make_pair(Symbol::down, std::string{});
                break;
            case KeyType::left:
                if (position > 0)
                {
                    out << '\b' << std::flush;
                    --position;
                }
                break;
            case KeyType::right:
                if (position < currentLine.size())
                {
                    out << profile.beforeInput
                        << currentLine[position]
                        << profile.afterInput << std::flush;
                    ++position;
                }
                break;
            case KeyType::ret:
            {
                out << "\r\n";
                auto cmd = currentLine;
                currentLine.clear();
                position = 0;
                return std::make_pair(Symbol::command, cmd);
            }
            break;
            case KeyType::ascii:
            {
                const char c = static_cast<char>(k.second);
                if (c == '\t')
                    return std::make_pair(Symbol::tab, std::string());
                else
                {
                    const auto pos = static_cast<std::string::difference_type>(position);

                    // output the new char:
                    out << profile.beforeInput << c;
                    // and the rest of the string:
                    out << std::string(currentLine.begin() + pos, currentLine.end())
                        << profile.afterInput;

                    // go back to the original position
                    out << std::string(currentLine.size() - position, '\b') << std::flush;

                    // update the buffer and cursor position:
                    currentLine.insert(currentLine.begin() + pos, c);
                    ++position;
                }

                break;
            }
            case KeyType::canc:
            {
                if (position == currentLine.size())
                    break;

                const auto pos = static_cast<std::string::difference_type>(position);

                // output the rest of the line
                out << std::string(currentLine.begin() + pos + 1, currentLine.end());
                // remove last char
                out << ' ';
                // go back to the original position
                out << std::string(currentLine.size() - position, '\b') << std::flush;
                // remove the char from buffer
                currentLine.erase(currentLine.begin() + pos);
                break;
            }
            case KeyType::end:
            {
                const auto pos = static_cast<std::string::difference_type>(position);

                out << profile.beforeInput
                    << std::string(currentLine.begin() + pos, currentLine.end())
                    << profile.afterInput << std::flush;
                position = currentLine.size();
                break;
            }
            case KeyType::home:
            {
                out << std::string(position, '\b') << std::flush;
                position = 0;
                break;
            }
            case KeyType::ignored:
                // TODO
                break;
        }

        return std::make_pair(Symbol::nothing, std::string());
    }

  private:
    std::string currentLine;
    std::size_t position = 0; // next writing position in currentLine
    std::ostream &out;
    const TerminalProfile& profile;
};

} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_TERMINAL_H_
