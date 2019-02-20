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

// 2019-01-30	DBJ	Created
// char runtime dynamic buffer type
// faster and lighter vs the usual suspects: string and vector
// note: this is part of the core , thus we will try and absolutely minimise
// the dependancies  thus we will sometimes
// reimplement things here which perhaps exist in other projects

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
#endif // ! DBJ_BUFFER_TESTING
/******************************************************************************/
namespace dbj {
	namespace buf {
		/*
		--------------------------------------------------------------------------------
		Above is for fast-and-dangerous code and bellow is a comfortable api
		runtime	char buffer.
		Slower than naked uniq_ptr but still approx 2x to 3x faster vs vector<char>
		I have the code for measuring the difference,
		in case you are hotly against *not* using vector<char>.
		Generaly the smaller the buffer the faster it is vs vector<char>.
		For *normal* buffer sizes (posix BUFSIZ or multiplies of it)
		using this type has a lot in favour.
		Also this class works in code where no exceptions are used
		please do let me know if problems there
		*/
				struct buffer final {
		
					using type = buffer;
					using pointer = buff_type;
					using value_type = char;
					using iterator = value_type * ;
					using reference_type = type & ;
		
					// default 
					buffer() { /* size is 0 here */ }

					// makes sized but empty buffer
					explicit buffer(inside_1_and_max size) noexcept 
					{ 
						this->reset(size); 
					}
		
					// copy
					buffer(const buffer & another_) noexcept
					{
						this->assign(another_);
					}

					buffer & operator =(const buffer & another_) noexcept
					{
						if (&another_ != this) {
							this->assign(another_);
						}
						return *this;
					}

					// construct from native charr array
					// i.e. the string literal
					template < typename T, size_t N ,
						std::enable_if_t< std::is_same_v<T, char> , int> = 0
					>
					buffer ( const T (& charr )[N] ) noexcept
					{
						assign (charr, charr + N );
					}
		
					void assign(const buffer & another_) const noexcept
					{
						this->reset(another_.size_);
						std::copy(another_.begin(), another_.end(), this->begin());
					}

					void assign(char const * from_, char const * to_) const noexcept
					{
						assert( from_ && to_ );
						this->reset(std::distance(from_, to_));
						std::copy(from_, to_, this->begin());
					}

					void reset(inside_1_and_max new_size_) const {
						(void)data_.release();
						this->size_ = new_size_;
						this->data_ = make_buff  (new_size_);
					}
		
					// notice the usage of the dbj::insider definition
					// as  argument type
					char & operator [] (inside_1_and_max idx_)
					{
						//  std::unique_ptr<T[]> has this operator
						return data_[idx_]; 
					}
		
					iterator data() const noexcept { return data_.get(); }
		
					size_t const & size() const noexcept { return size_; }
		
					value_type ** const address() const noexcept {
						auto p = std::addressof(data_[0]);
						return std::addressof(p);
					}

					iterator begin() noexcept { return data_.get(); }
					iterator end()   noexcept { return data_.get() + size_; }
					const iterator begin() const noexcept { return data_.get(); }
					const iterator end()   const noexcept { return data_.get() + size_; }
		
					buffer const & fill(char val_) const noexcept
					{
						::dbj::buf::fill( this->data_, val_, this->size());
						return *this;
					}
		
					// to avoid "never releasing assign pointer" syndrome
					// we wil ban creation of this class on the heap
					void* operator new(std::size_t sz) = delete;
					void* operator new[](std::size_t sz) = delete;
					void  operator delete(void* ptr, std::size_t sz) = delete;
					void  operator delete[](void* ptr, std::size_t sz) = delete;

				private:

					mutable inside_1_and_max  size_{}; // 0
					mutable	pointer data_{}; // size == 0

//  buffer friends start here
		
					friend std::string to_string(const reference_type from_) noexcept {
						// this will not copy the whole buffer
						return { from_.data() };
					}
		
					friend std::vector<char> to_vector(const reference_type from_) noexcept {
						// this will not copy the whole buffer
						std::string str_(from_.data());
						return { str_.begin(), str_.end() };
					}

					//  assignments are used to make "from" certain types
					friend void assign (reference_type target_, 
						char const * from_, char const * to_ )
					{
						target_.assign( from_, to_ );
					}

					friend void assign(reference_type target_, std::string str_)
					{
						// naughty ;)
						target_.assign( (char *)str_.data(), (char *)(str_.data() + str_.size()));
					}

					friend void assign(reference_type target_, dbj::string_view strvw_)
					{
						// naughty ;)
						target_.assign((char *)strvw_.data(), (char *)(strvw_.data() + strvw_.size()));
					}

					friend void assign(reference_type target_, std::vector<char> charvec_)
					{
						// naughty ;)
						target_.assign((char *)charvec_.data(), (char *)(charvec_.data() + charvec_.size()));
					}
		
					friend
						bool operator == (const reference_type left_, const reference_type right_)
					noexcept
					{
						if (left_.size() != right_.size())
							return false ;
		
						return std::equal (
							left_.begin(), left_.end(),
							right_.begin(), right_.end()
							);
					}
		
					friend std::ostream & operator << (std::ostream & os, buffer const & cb_ ) 
					{
						return os << cb_.data_.get();
					}
		
//  buffer friends end here
				}; // buffer
		
	} // buf
} // dbj


#ifdef DBJ_BUFFER_TESTING

namespace {
	inline bool dbj_testing_dbj_comfy_buffer()
	{
		using namespace ::dbj::buf;
		// 1
		{
			TU(buffer(BUFSIZ));

			TU(buffer("string literall"));
			
			buffer bf(BUFSIZ);
			assign(bf, std::string("std::string"));
			TU(bf);
			assign(bf, dbj::string_view("std::stringview"));
			TU(bf);
		}
		// 2
		{
			buffer		 buff_1 = buffer(0xF);
			TU( buff_1.fill( '*' )) ;
			TU(buff_1);
			buffer		 buff_2 = buff_1;
			TU(buff_1 == buff_2);
		}
		return true;
	} // testing_dbj_buffer

	static auto dbj_testing_dbj_comfy_buffer_result
		= dbj_testing_dbj_comfy_buffer();
}

#endif // DBJ_BUFFER_TESTING
#undef TU
#undef DBJ_BUFFER_TESTING
