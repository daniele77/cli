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

/*
	--------------------------------------------------------------------------------

	2019 FEB	dbj@dbj.org

	dbj::buf is for fast-and-dangerous code and bellow is a bit more
	comfortable api for	dynamic	char buffer.
	Slower than naked uniq_ptr<char[]> but still approx 2x to 3x faster vs vector<char>
	
	btw: I have the code for measuring the difference,in case you are hotly 
	against *not* using vector<char>.

	Generaly the smaller the buffer the faster it is vs vector<char>.
	For *normal* buffer sizes (posix BUFSIZ or multiplies of it)
	using this type has a lot in favour.
*/

namespace dbj {
	namespace buf {
	
				struct buffer final {
		
					using type = buffer;

					// alias for std::unique_ptr<char[]>
					using pointer = typename ::dbj::buf::buff_type;

					using value_type = char;
					using iterator = value_type * ;
					using reference_type = type & ;
		
					// default 
					buffer() { /* size is 0 here */ }

					// sized but empty buffer
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
#define TU(x) std::cout << "\nExpression: '" << (#x) << "'\n\tResult: '" << (x) << "'\n"
#include <iostream>
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
