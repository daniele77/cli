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

#ifndef CLI_DETAIL_GENERICCLIASYNCSESSION_H_
#define CLI_DETAIL_GENERICCLIASYNCSESSION_H_

#include <string>
#include "../cli.h" // CliSession
#include "genericasioscheduler.h"

namespace cli
{
namespace detail
{

template <typename ASIOLIB>
class GenericCliAsyncSession : public CliSession
{
public:
    GenericCliAsyncSession(GenericAsioScheduler<ASIOLIB>& _scheduler, Cli& _cli) :
        CliSession(_cli, std::cout, 1),
        input(_scheduler.AsioContext(), ::dup(STDIN_FILENO))
    {
        Read();
    }
    ~GenericCliAsyncSession() noexcept override
    {
        try { input.close(); } catch (const std::exception&) { /* do nothing */ }
    }

private:

    void Read()
    {
        Prompt();
        // Read a line of input entered by the user.
        asiolib::async_read_until(
            input,
            inputBuffer,
            '\n',
            std::bind( &GenericCliAsyncSession::NewLine, this,
                       std::placeholders::_1,
                       std::placeholders::_2 )
        );
    }

    void NewLine(const asiolibec::error_code& error, std::size_t length )
    {
        if ( !error || error == asiolib::error::not_found )
        {
            auto bufs = inputBuffer.data();
            auto size = static_cast<long>(length);
            if ( !error ) --size; // remove \n
            std::string s(asiolib::buffers_begin( bufs ), asiolib::buffers_begin( bufs ) + size);
            inputBuffer.consume( length );

            Feed( s );
            Read();
        }
        else
        {
            input.close();
        }
    }

    asiolib::streambuf inputBuffer;
    asiolib::posix::stream_descriptor input;
};

} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_GENERICCLIASYNCSESSION_H_

