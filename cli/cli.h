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

#pragma once 

#ifndef CLI_H_
#define CLI_H_

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "dbj/lexical_cast.h"
#include "dbj/algorithm_string.h"
#include "colorprofile.h"
#include "history.h"

namespace cli
{
	using namespace ::std;
	using string_vector = vector<string>;

    // ********************************************************************
    template < typename T > struct type_descriptor {};
    template <> struct type_descriptor< char > { static const char* Name() { return "<char>"; } };
    template <> struct type_descriptor< unsigned char > { static const char* Name() { return "<unsigned char>"; } };
    template <> struct type_descriptor< short > { static const char* Name() { return "<short>"; } };
    template <> struct type_descriptor< unsigned short > { static const char* Name() { return "<unsigned short>"; } };
    template <> struct type_descriptor< int > { static const char* Name() { return "<int>"; } };
    template <> struct type_descriptor< unsigned int > { static const char* Name() { return "<unsigned int>"; } };
    template <> struct type_descriptor< long > { static const char* Name() { return "<long>"; } };
    template <> struct type_descriptor< unsigned long > { static const char* Name() { return "<unsigned long>"; } };
    template <> struct type_descriptor< float > { static const char* Name() { return "<float>"; } };
    template <> struct type_descriptor< double > { static const char* Name() { return "<double>"; } };
    template <> struct type_descriptor< long double > { static const char* Name() { return "<long double>"; } };
    template <> struct type_descriptor< bool > { static const char* Name() { return "<bool>"; } };
    template <> struct type_descriptor< std::string > { static const char* Name() { return "<string>"; } };
    // ********************************************************************

// forward declarations
    class menu_type;
    class cli_session_type;

    class cli_type
    {

        // inner class to provide a global output stream
        class out_stream
        {
        public:
            template <typename T>
            out_stream& operator << (const T& msg)
            {
                for (auto out: ostreams)
                    *out << msg;
                return *this;
            }

            // this is the type of std::cout
            typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
            // this is the function signature of std::endl
            typedef CoutType& (*StandardEndLine)(CoutType&);

            // takes << std::endl
            out_stream& operator << (StandardEndLine manip)
            {
                for (auto out: ostreams)
                    manip(*out);
                return *this;
            }

        private:
            friend class cli_type;

            void Register(std::ostream& o)
            {
                ostreams.push_back(&o);
            }
            void UnRegister(std::ostream& o)
            {
                ostreams.erase(std::remove(ostreams.begin(), ostreams.end(), &o), ostreams.end());
            }

            std::vector<std::ostream*> ostreams;
        };
        // end inner class

    public:
        cli_type(
            std::unique_ptr< menu_type >&& _rootMenu,
            std::function< void( std::ostream& )> _exitAction = std::function< void(std::ostream&) >()
        ) :
            rootMenu( std::move(_rootMenu) ),
            exitAction( _exitAction )
        {
        }


        // disable value semantics
        cli_type( const cli_type& ) = delete;
        cli_type& operator = ( const cli_type& ) = delete;

        menu_type* RootMenu() { return rootMenu.get(); }
        void ExitAction( std::function< void(std::ostream&)> action ) { exitAction = action; }
        void ExitAction( std::ostream& out ) { if ( exitAction ) exitAction( out ); }

        static void Register(std::ostream& o) { cout().Register(o); }
        static void UnRegister(std::ostream& o) { cout().UnRegister(o); }

        static out_stream& cout() { static out_stream s; return s; }

    private:
        std::unique_ptr< menu_type > rootMenu; // just to keep it alive
        std::function< void(std::ostream&) > exitAction;
    };

    // ********************************************************************

    class command_type
    {
    public:
        explicit command_type(const std::string& _name) : name(_name) {}
        virtual ~command_type() = default;
        virtual bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) = 0;
        virtual void Help( std::ostream& out ) const = 0;
        // Returns the collection of completions relatives to this command.
        // For simple commands, provides a base implementation that use the name of the command
        // for aggregate commands (i.e., menu_type), the function is redefined to give the menu command
        // and the subcommand recursively
        virtual string_vector GetCompletionRecursive(const std::string& line) const
        {
            if ( dbj::algorithm::starts_with(name, line) ) return {name};
            else return {};
        }
    protected:
        const std::string& Name() const { return name; }
    private:
        const std::string name;
    };

    // ********************************************************************

    // free utility function to get completions from a list of commands and the current line
    inline string_vector GetCompletions(const std::vector< std::unique_ptr< command_type > >& cmds, const std::string& currentLine)
    {
        string_vector result;
        std::for_each( cmds.begin(), cmds.end(),
            [&currentLine,&result](auto& cmd)
            {
                auto c = cmd->GetCompletionRecursive(currentLine);
                result.insert(result.end(), std::make_move_iterator(c.begin()), std::make_move_iterator(c.end()));
            }
        );
        return result;
    }

    // ********************************************************************

    class cli_session_type
    {
    public:
        cli_session_type( cli_type& _cli, std::ostream& _out, std::size_t historySize = 100 );
        ~cli_session_type() { cli.UnRegister(out); }

        // disable value semantics
        cli_session_type( const cli_session_type& ) = delete;
        cli_session_type& operator = ( const cli_session_type& ) = delete;

        void Feed( const std::string& cmd );

        void Prompt();

        void Current( menu_type* menu )
        {
            current = menu;
        }

        std::ostream& out_stream() { return out; }

        void Help() const;

        void Exit()
        {
            if (exitAction) exitAction(out);
            cli.ExitAction(out);
        }

        void ExitAction( std::function< void(std::ostream&)> action )
        {
            exitAction = action;
        }

        void ShowHistory() const { history.Show(out); }

        std::string PreviousCmd(const std::string& line)
        {
            return history.Previous(line);
        }

        std::string NextCmd()
        {
            return history.Next();
        }

        string_vector GetCompletions(const std::string& currentLine) const;

    private:

        cli_type& cli;
        menu_type* current;
        std::unique_ptr< menu_type > globalScopeMenu;
        std::ostream& out;
        std::function< void(std::ostream&)> exitAction;
        detail::History history;
    };

    // ********************************************************************

    class menu_type : public command_type
    {
    public:
        // disable value semantics
        menu_type( const menu_type& ) = delete;
        menu_type& operator = ( const menu_type& ) = delete;

        menu_type() : command_type( {} ), parent( nullptr ), description() {}

        menu_type( const std::string& _name, const std::string& desc = "(menu)" ) :
            command_type( _name ), parent( nullptr ), description( desc )
        {}

        template < typename F >
        void Add( const std::string& name, F f, const std::string& help = "" )
        {
            // dispatch to private Add methods
            Add( name, help, f, &F::operator() );
        }

        void Add( std::unique_ptr< command_type >&& cmd )
        {
            cmds.push_back( std::move(cmd) );
        }

        void Add( std::unique_ptr< menu_type >&& menu )
        {
            menu -> parent = this;
            cmds.push_back( std::move(menu) );
        }

        bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) override
        {
            if ( cmdLine[ 0 ] == Name() )
            {
                if ( cmdLine.size() == 1 )
                {
                    session.Current( this );
                    return true;
                }
                else
                {
                    // check also for subcommands
                    std::vector<std::string > subCmdLine( cmdLine.begin()+1, cmdLine.end() );
                    for ( auto& cmd: cmds )
                        if ( cmd -> Exec( subCmdLine, session ) ) return true;
                }
            }
            return false;
        }

        bool ScanCmds( const std::vector< std::string >& cmdLine, cli_session_type& session )
        {
            for ( auto& cmd: cmds )
                if ( cmd -> Exec( cmdLine, session ) ) return true;
            if ( parent && parent -> Exec( cmdLine, session ) ) return true;
            return false;
        }

        std::string Prompt() const
        {
            return Name();
        }

        void MainHelp( std::ostream& out )
        {
            for ( auto& cmd: cmds )
                cmd -> Help( out );
            if ( parent ) parent -> Help( out );
        }

        void Help( std::ostream& out ) const override
        {
            out << " - " << Name() << "\n\t" << description << "\n";
        }

        string_vector GetCompletions(const std::string& currentLine) const
        {
            auto result = cli::GetCompletions(cmds, currentLine);
			if (parent)
			{
				auto c = parent->GetCompletionRecursive(currentLine);
				result.insert( result.end(), std::make_move_iterator(c.begin()), std::make_move_iterator(c.end()));
			}
			return result;
        }

        virtual string_vector GetCompletionRecursive(const std::string& line) const override
        {
            if ( dbj::algorithm::starts_with( line, Name() ) )
            {
                auto rest = line;
                rest.erase( 0, Name().size() );
               dbj::algorithm::trim_left(rest);
                string_vector result;
                for ( auto& cmd: cmds )
                {
                    auto cs = cmd->GetCompletionRecursive( rest );
                    for ( auto& c: cs )
                        result.push_back( Name() + ' ' + c );
                }
                return result;
            }
            return command_type::GetCompletionRecursive(line);
        }

    private:

        template < typename F, typename R >
        void Add( const std::string& name, const std::string& help, F& f,R (F::*mf)(std::ostream& out) const );

        template < typename F, typename R, typename A1 >
        void Add( const std::string& name, const std::string& help, F& f,R (F::*mf)(A1, std::ostream& out) const );

        template < typename F, typename R, typename A1, typename A2 >
        void Add( const std::string& name, const std::string& help, F& f,R (F::*mf)(A1, A2, std::ostream& out) const );

        template < typename F, typename R, typename A1, typename A2, typename A3 >
        void Add( const std::string& name, const std::string& help, F& f,R (F::*mf)(A1, A2, A3, std::ostream& out) const );

        template < typename F, typename R, typename A1, typename A2, typename A3, typename A4 >
        void Add( const std::string& name, const std::string& help, F& f,R (F::*mf)(A1, A2, A3, A4, std::ostream& out) const );

        menu_type* parent;
        const std::string description;
        using Cmds = std::vector< std::unique_ptr< command_type > >;
        Cmds cmds;
    };

    // ********************************************************************

    class func_cmd : public command_type
    {
    public:
        // disable value semantics
        func_cmd( const func_cmd& ) = delete;
        func_cmd& operator = ( const func_cmd& ) = delete;

        func_cmd(
            const std::string& _name,
            std::function< void( std::ostream& )> _function,
            const std::string& desc = ""
        ) : command_type( _name ), function( _function ), description( desc )
        {
        }
        bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) override
        {
            if ( cmdLine.size() != 1 ) return false;
            if ( cmdLine[ 0 ] == Name() )
            {
                function( session.out_stream() );
                return true;
            }

            return false;
        }
        void Help( std::ostream& out ) const override
        {
            out << " - " << Name() << "\n\t" << description << "\n";
        }
    private:
        const std::function< void( std::ostream& )> function;
        const std::string description;
    };

    template < typename T >
    class func_cmd_1 : public command_type
    {
    public:
        // disable value semantics
        func_cmd_1( const func_cmd_1& ) = delete;
        func_cmd_1& operator = ( const func_cmd_1& ) = delete;

        func_cmd_1(
            const std::string& _name,
            std::function< void( T, std::ostream& ) > _function,
            const std::string& desc = ""
            ) : command_type( _name ), function( _function ), description( desc )
        {
        }
        bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) override
        {
            if ( cmdLine.size() != 2 ) return false;
            if ( Name() == cmdLine[ 0 ] )
            {
                try
                {
                    // T arg = boost::lexical_cast<T>( cmdLine[ 1 ] );
                    T arg = dbj::stoi( cmdLine[ 1 ] );
                    function( arg, session.out_stream() );
                }
                catch ( dbj::bad_lexical_cast & )
                {
                    return false;
                }
                return true;
            }

            return false;
        }
        void Help( std::ostream& out ) const override
        {
            out << " - " << Name()
                << " " << type_descriptor< T >::Name()
                << "\n\t" << description << "\n";
        }
    private:
        const std::function< void( T, std::ostream& )> function;
        const std::string description;
    };

    template < typename T1, typename T2 >
    class func_cmd_2 : public command_type
    {
    public:
        // disable value semantics
        func_cmd_2( const func_cmd_2& ) = delete;
        func_cmd_2& operator = ( const func_cmd_2& ) = delete;

        func_cmd_2(
            const std::string& _name,
            std::function< void( T1, T2, std::ostream& ) > _function,
            const std::string& desc = "2 parameter command"
            ) : command_type( _name ), function( _function ), description( desc )
        {
        }
        bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) override
        {
            if ( cmdLine.size() != 3 ) return false;
            if ( Name() == cmdLine[ 0 ] )
            {
                try
                {
                    T1 arg1 = dbj::lexical_cast<T1>( cmdLine[ 1 ] );
                    T2 arg2 = dbj::lexical_cast<T2>( cmdLine[ 2 ] );
                    function( arg1, arg2, session.out_stream() );
                }
                catch ( dbj::bad_lexical_cast & )
                {
                    return false;
                }
                return true;
            }

            return false;
        }
        void Help( std::ostream& out ) const override
        {
            out << " - " << Name()
                << " " << type_descriptor< T1 >::Name()
                << " " << type_descriptor< T2 >::Name()
                << "\n\t" << description << "\n";
        }
    private:
        const std::function< void( T1, T2, std::ostream& )> function;
        const std::string description;
    };

    template < typename T1, typename T2, typename T3 >
    class func_cmd_3 : public command_type
    {
    public:
        // disable value semantics
        func_cmd_3( const func_cmd_3& ) = delete;
        func_cmd_3& operator = ( const func_cmd_3& ) = delete;

        func_cmd_3(
            const std::string& _name,
            std::function< void( T1, T2, T3, std::ostream& ) > _function,
            const std::string& desc = "3 parameters command"
            ) : command_type( _name ), function( _function ), description( desc )
        {
        }
        bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) override
        {
            if ( cmdLine.size() != 4 ) return false;
            if ( Name() == cmdLine[ 0 ] )
            {
                try
                {
                    T1 arg1 = dbj::lexical_cast<T1>( cmdLine[ 1 ] );
                    T2 arg2 = dbj::lexical_cast<T2>( cmdLine[ 2 ] );
                    T3 arg3 = dbj::lexical_cast<T3>( cmdLine[ 3 ] );
                    function( arg1, arg2, arg3, session.out_stream() );
                }
                catch ( dbj::bad_lexical_cast & )
                {
                    return false;
                }
                return true;
            }

            return false;
        }
        void Help( std::ostream& out ) const override
        {
            out << " - " << Name()
                << " " << type_descriptor< T1 >::Name()
                << " " << type_descriptor< T2 >::Name()
                << " " << type_descriptor< T3 >::Name()
                << "\n\t" << description << "\n";
        }
    private:
        const std::function< void( T1, T2, T3, std::ostream& )> function;
        const std::string description;
    };

    template < typename T1, typename T2, typename T3, typename T4 >
    class func_cmd_4 : public command_type
    {
    public:
        // disable value semantics
        func_cmd_4( const func_cmd_4& ) = delete;
        func_cmd_4& operator = ( const func_cmd_4& ) = delete;

        func_cmd_4(
            const std::string& _name,
            std::function< void( T1, T2, T3, T4, std::ostream& ) > _function,
            const std::string& desc = "4 parameters command"
            ) : command_type( _name ), function( _function ), description( desc )
        {
        }
        bool Exec( const std::vector< std::string >& cmdLine, cli_session_type& session ) override
        {
            if ( cmdLine.size() != 5 ) return false;
            if ( Name() == cmdLine[ 0 ] )
            {
                try
                {
                    T1 arg1 = dbj::lexical_cast<T1>( cmdLine[ 1 ] );
                    T2 arg2 = dbj::lexical_cast<T2>( cmdLine[ 2 ] );
                    T3 arg3 = dbj::lexical_cast<T3>( cmdLine[ 3 ] );
                    T4 arg4 = dbj::lexical_cast<T4>( cmdLine[ 4 ] );
                    function( arg1, arg2, arg3, arg4, session.out_stream() );
                }
                catch ( dbj::bad_lexical_cast & )
                {
                    return false;
                }
                return true;
            }

            return false;
        }
        void Help( std::ostream& out ) const override
        {
            out << " - " << Name()
                << " " << type_descriptor< T1 >::Name()
                << " " << type_descriptor< T2 >::Name()
                << " " << type_descriptor< T3 >::Name()
                << " " << type_descriptor< T4 >::Name()
                << "\n\t" << description << "\n";
        }
    private:
        const std::function< void( T1, T2, T3, T4, std::ostream& )> function;
        const std::string description;
    };

    // ********************************************************************

    // cli_session_type implementation

	// DBJ: why is this not in-line implementation?

    inline cli_session_type::cli_session_type(cli_type& _cli, std::ostream& _out, std::size_t historySize) :
            cli(_cli),
            current(cli.RootMenu()),
            globalScopeMenu(std::make_unique< menu_type >()),
            out(_out),
            history(historySize)
        {
            cli.Register(out);
            globalScopeMenu->Add(
                "help",
                [this](std::ostream&){ Help(); },
                "This help message"
            );
            globalScopeMenu->Add(
                "exit",
                [this](std::ostream&){ Exit(); },
                "Quit the session"
            );
#ifdef CLI_HISTORY_CMD
            globalScopeMenu->Add(
                "history",
                [this](std::ostream&){ ShowHistory(); },
                "Show the history"
            );
#endif
        }

    inline void cli_session_type::Feed( const std::string& cmd )
    {
		/*
        std::vector< std::string > strs;
        dbj::split( strs, cmd, dbj::is_any_of( " \t\n" ), dbj::token_compress_on );

		replaced the above with bellow:
		*/
		string_vector strs = dbj::algorithm::fast_string_split(cmd, " \t\n");
        // remove null entries from the vector:
        strs.erase(
            std::remove_if(
                strs.begin(),
                strs.end(),
                [](const std::string& s){ return s.empty(); }
            ),
            strs.end()
        );
        if ( strs.empty() ) return; // just hit enter

        // global cmds check
        bool found = globalScopeMenu->ScanCmds(strs, *this);

        // root menu recursive cmds check
        if ( !found ) found = current -> ScanCmds( std::move(strs), *this ); // last use of strs

        history.NewCommand( cmd ); // add anyway to history
        if ( !found ) // error msg if not found
            out << "command_type unknown: " << cmd << "\n";

        return;
    }

    inline void cli_session_type::Prompt()
    {
        out << beforePrompt
            << current -> Prompt()
            << afterPrompt
            << "> "
            << std::flush;
    }

    inline void cli_session_type::Help() const
    {
        out << "Commands available:\n";
        globalScopeMenu->MainHelp(out);
        current -> MainHelp( out );
    }

    inline string_vector cli_session_type::GetCompletions( const std::string& currentLine ) const
    {
        auto v1 = globalScopeMenu->GetCompletions(currentLine);
        auto v3 = current -> GetCompletions(currentLine);
        v1.insert( v1.end(), std::make_move_iterator(v3.begin()), std::make_move_iterator(v3.end()) );
        return v1;
    }

    // menu_type implementation

    template < typename F, typename R >
    void menu_type::Add( const std::string& name, const std::string& help, F& f,R (F::*)(std::ostream& out) const )
    {
        cmds.push_back( std::make_unique< func_cmd >( name, f, help ) );
    }

    template < typename F, typename R, typename A1 >
    void menu_type::Add( const std::string& name, const std::string& help, F& f,R (F::*)(A1, std::ostream& out) const )
    {
        cmds.push_back( std::make_unique< func_cmd_1< A1 > >( name, f, help ) );
    }

    template < typename F, typename R, typename A1, typename A2 >
    void menu_type::Add( const std::string& name, const std::string& help, F& f,R (F::*)(A1, A2, std::ostream& out) const )
    {
        cmds.push_back( std::make_unique< func_cmd_2< A1, A2 > >( name, f, help ) );
    }

    template < typename F, typename R, typename A1, typename A2, typename A3 >
    void menu_type::Add( const std::string& name, const std::string& help, F& f,R (F::*)(A1, A2, A3, std::ostream& out) const )
    {
        cmds.push_back( std::make_unique< func_cmd_3< A1, A2, A3 > >( name, f, help ) );
    }

    template < typename F, typename R, typename A1, typename A2, typename A3, typename A4 >
    void menu_type::Add( const std::string& name, const std::string& help, F& f,R (F::*)(A1, A2, A3, A4, std::ostream& out) const )
    {
        cmds.push_back( std::make_unique< func_cmd_4< A1, A2, A3, A4> >( name, f, help ) );
    }

} // namespace

#endif
