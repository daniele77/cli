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

#ifndef CLI_DETAIL_GENERICASIOSCHEDULER_H_
#define CLI_DETAIL_GENERICASIOSCHEDULER_H_

#include "../scheduler.h"
#include <memory> // unique_ptr

namespace cli
{
namespace detail
{

template <typename ASIOLIB>
class GenericAsioScheduler : public Scheduler
{
public:

    using ContextType = typename ASIOLIB::ContextType;
    using WorkGuard = typename ASIOLIB::WorkGuard;

    GenericAsioScheduler() :
        owned{true},
        context{new ContextType()},
        executor{*context},
        work{std::make_unique<WorkGuard>(ASIOLIB::MakeWorkGuard(*context))}
    {}

    explicit GenericAsioScheduler(ContextType& _context) : context{&_context}, executor{*context} {}

    ~GenericAsioScheduler() override { if (owned) delete context; }

    // non copyable
    GenericAsioScheduler(const GenericAsioScheduler&) = delete;
    GenericAsioScheduler& operator=(const GenericAsioScheduler&) = delete;

    void Stop() { context->stop(); }

    void Run()
    {
        context->run();
    }

    void ExecOne() { context->run_one(); }

    void PollOne() { context->poll_one(); }

    void Post(const std::function<void()>& f) override
    {
        executor.Post(f);
    }

    ContextType& AsioContext() { return *context; }

private:

    using ExecutorType = typename ASIOLIB::Executor;

    bool owned = false;
    ContextType* context;
    ExecutorType executor;
    std::unique_ptr<WorkGuard> work;
};


} // namespace detail
} // namespace cli

#endif // CLI_DETAIL_GENERICASIOSCHEDULER_H_
