#pragma once

#include "LICENCE.h"
#include "dbj_buffer.h"

#define DBJ_BUFFER_TESTING 

namespace dbj {
	namespace fmt {
		/*
		somewhat inspired with
		https://msdn.microsoft.com/en-us/magazine/dn913181.aspx
		*/
		template <typename T>
		inline T const & frm_arg(T const & value) noexcept
		{
			return value;
		}
		// no pass by value allowed
		template <typename T> T & frm_arg(T && value) = delete;

		// template <typename T>
		inline char * frm_arg(std::unique_ptr<char[]> const & value) noexcept
		{
			return value.get();
		}

		inline wchar_t * frm_arg(std::unique_ptr<wchar_t[]> const & value) noexcept
		{
			return value.get();
		}

		//inline char * frm_arg(::dbj::buf::char_buffer const & value) noexcept
		//{
		//	return value.data();
		//}

#pragma endregion 

		template <typename T>
		inline T const * frm_arg(std::basic_string<T> const & value) noexcept
		{
			return value.c_str();
		}

		inline char const * frm_arg(dbj::string_view const & value) noexcept
		{
			return value.data();
		}
		/*
		vaguely inspired by
		https://stackoverflow.com/a/39972671/10870835
		*/
		template<typename ... Args>
		inline std::unique_ptr<char[]>
			format (char const * format_, Args const & ...args)
			noexcept
		{
			assert(format_);
			// max 255 arguments allowed
			assert(sizeof...(args) < 255 );
			const auto fmt = format_ ;
			// 1: what is he size required
			const size_t size = 1 + std::snprintf(nullptr, 0, fmt, frm_arg(args) ...);
			// 2: use it at runtime
			auto buf = std::make_unique<char[]>(size + 1);
			// each arg becomes arg to the frm_arg() overload found
			std::snprintf(buf.get(), size, fmt, frm_arg(args) ...);

			return buf;
		}
#if !( __cplusplus < 201703)
		// wide version uses std::wstring_view
		// for which we have no substitue in this project
		template<typename ... Args>
		inline std::unique_ptr<wchar_t[]>
			format(std::wstring_view format_, Args const & ...args)
			noexcept
		{
			static_assert(sizeof...(args) < 255, "\n\nmax 255 arguments allowed\n");
			const auto fmt = format_.data();
			// 1: what is he size required
			const size_t size = 1 + std::swprintf(nullptr, 0, fmt, frm_arg(args) ...);
			// 2: use it at runtime
			auto buf = std::make_unique<wchar_t[]>(size + 1);
			// each arg becomes arg to the frm_arg() overload found
			std::swprintf(buf.get(), size, fmt, frm_arg(args) ...);

			return buf;
		}
#endif

	} // fmt

		template<typename ... Args>
		inline void
			prinf(char const * format_, Args const & ... args)	noexcept
		{
			std::printf("%s", fmt::format(format_, args...).get());
		}

} // dbj

#ifdef DBJ_BUFFER_TESTING
#define TU(x) std::cout << "\nExpression: '" << (#x) << "'\n\tResult: '" << (x) << "'\n"
#include <iostream>

namespace {
	inline bool dbj_testing_dbj_format ()
	{
		using namespace ::dbj::buf;
		// 1
		{
			TU( ::dbj::fmt::format("%s %d", "string literal", 42) );
			TU( ::dbj::fmt::format("%s", std::string("std::string")));
			TU( ::dbj::fmt::format("%s", dbj::string_view("std::stringview")));

			auto buf = assign("dbj buf unique_ptr<char[]>");
			TU(::dbj::fmt::format("%s", buf));
		}		
		// 2
		{
			dbj::prinf("\n\n%s %d", "string literal", 42) ;
			dbj::prinf("\n%s", std::string("std::string"));
			dbj::prinf("\n%s", dbj::string_view("std::stringview"));

			auto buf = assign("dbj buf unique_ptr<char[]>");
			dbj::prinf("\n%s\n\n", buf);
		}
		return true;
	} // testing_dbj_buffer

	static auto ddbj_testing_dbj_format_result
		= dbj_testing_dbj_format();
}
#endif // ! DBJ_BUFFER_TESTING
#undef TU
#undef DBJ_BUFFER_TESTING