#pragma once

#define DBJ_BUFFER_TESTING

#include "LICENCE.h"
#include "dbj_insider.h"

#include <system_error>
#include <cassert>
#include <memory>
#include <string_view>
#include <type_traits>
#include <cstdint>
#include <cstddef>
#include <cstring>

// 2019-01-30	dbj@dbj.org	created
//
// char runtime dynamic buffer type
// faster and lighter vs the usual suspects: string and vector
// note: this is part of the core , thus we will try and absolutely minimise
// the dependancies  thus we will sometimes
// reimplement things here which perhaps exist in other projects
//
// note: this is not C++17 way of doing things
// so that good legacy code can enjoy this too

namespace dbj {
		namespace buf {
#if __cplusplus < 201703
		using bpstd::string_view; // back-port std project
#endif
		// plenty of space -- 65535
		// in posix terms BUFSIZ * 2 * 64 aka 64KB
		constexpr std::size_t max_length = UINT16_MAX;

		namespace {
			using namespace ::dbj::util;
			using inside_1_and_max =
				insider<size_t, 1,
				::dbj::buf::max_length
				, insider_error_code_throw_policy
				>;
		}

		struct unique_arr_ptr final
		{
			using value_type = char;
			using type = std::unique_ptr<char[]>;
			using ref_type = type & ;
		};

		using buff_type		= typename unique_arr_ptr::type;
		using buff_ref_type = typename unique_arr_ptr::ref_type;

		// always use this function to make_buff  fresh buff_type-es!
		DBJ_NODISCARD  inline buff_type make_buff (inside_1_and_max size_) noexcept
		{
			return std::make_unique<char[]>(size_ + 1);
		}

		DBJ_NODISCARD  inline auto length(buff_ref_type buf_ref_)
			noexcept -> inside_1_and_max
		{
			return std::strlen(buf_ref_.get());
		}

		DBJ_NODISCARD  inline buff_type assign(char const * first_, char const * last_) noexcept
		{
			assert( first_ && last_ );
			size_t N = std::distance( first_, last_ );
			assert(N > 0);
			buff_type sp_ = make_buff(N);
			auto * rez_ =  std::copy(first_, last_, sp_.get() );
			assert(rez_);
			return sp_; // the move
		}

		/*
		make_buff  from array of char's
		*/
		template<size_t N>
		DBJ_NODISCARD  inline auto assign(const char(&charr)[N]) noexcept -> buff_type
		{
			assert(N > 0);
			buff_type sp_ = make_buff (N);
			void * rez_ = ::memcpy(sp_.get(), charr, N);
			assert(rez_);
			return sp_; // the move
		}

		/* make_buff  buff_type from string_view */
		DBJ_NODISCARD  inline  auto	assign	( string_view sv_) noexcept	-> buff_type
		{
			assert(!sv_.empty());
			// is this naive?
			// should I use strnlen() ?
			inside_1_and_max N = sv_.size();
			buff_type sp_ = make_buff  (N);
			void * rez_ = ::memcpy(sp_.get(), sv_.data(), N);
			assert(rez_);
			return sp_;
		}

		/*
		make_buff  buff_type<C> from buff_type<C>
		this is clever since we do not deal 
		with C *, but... can we rely on the the caller
		making a properly zero terminated string in there
		so that strlen will work?
		*/
		DBJ_NODISCARD  inline auto	assign (buff_ref_type buf_ref_) noexcept -> buff_type
		{
			buff_type & from_ = buf_ref_;
			assert(from_);
			inside_1_and_max N = ::dbj::buf::length (buf_ref_);
			assert(N > 1);
			buff_type sp_ = ::dbj::buf::make_buff  (N);
			assert(sp_);
			void * rez_ = ::memcpy(sp_.get(), buf_ref_.get(), N);
			assert(rez_);
			return sp_;
		}

		/*
		assign array to instance of unique_ptr<T[]>
		note: "any" type will do as long as std::unique_ptr
		will accept it
		*/
		template<typename C, size_t N>
		inline auto	assign ( std::unique_ptr<C[]> & sp_, const C(&arr)[N]) noexcept -> std::unique_ptr<C[]> &
		{
			// sp_.release();
			sp_ = std::make_unique<C[]>(N + 1);
			assert(sp_);
			void * rez_ = ::memcpy(sp_.get(), arr, N);
			assert(rez_);
			return sp_;
		}

		// do not try this at home. ever.
		extern "C"	inline void	secure_reset(void *s, size_t n) noexcept
		{
			volatile char *p = (char *)s;
			while (n--) *p++ = 0;
		}

		// dangerously low, but it works
		inline  buff_ref_type fill( buff_ref_type buff_, char val_ , size_t N = 0 ) noexcept
		{
			if (!N) N = ::dbj::buf::length(buff_);
			::std::fill(buff_.get(), buff_.get() + N, val_ );
			return buff_;
		}

		inline std::ostream & operator << (	std::ostream & os, buff_ref_type the_buffer_ )
		{
			return os << the_buffer_.get();
		}

	} // buf
} // dbj

#ifdef DBJ_BUFFER_TESTING
#define TU(x) std::cout << "\nExpression: '" << (#x) << "'\n\tResult: '" << (x) << "'\n"
#include <iostream>

namespace {
	inline bool dbj_testing_dbj_unique_ptr_buffer() 
	{
		using namespace ::dbj::buf;
		// 1
		{
			TU( make_buff (BUFSIZ) );
			TU( assign("string literall") );
			TU( assign(std::string("std::string")) );
			TU( assign(dbj::string_view("std::stringview")) );

		}
		// 2
		{
			buff_type		 buff_1 = make_buff(0xF);
			// buff_type is a smart pointer 
			// it does not know its own safe length
			TU( fill( buff_1  , '*', 0xF ) ) ;
		}
		return true;
	} // testing_dbj_buffer

	static auto dbj_testing_dbj_buffer_result 
		= dbj_testing_dbj_unique_ptr_buffer();
}

#endif // DBJ_BUFFER_TESTING
#undef TU
#undef DBJ_BUFFER_TESTING
