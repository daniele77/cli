#pragma once

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
		namespace {

			// plenty of space -- 65535
			// in posix terms BUFSIZ * 2 * 64 aka 64KB
			constexpr std::size_t max_length = UINT16_MAX;

			using namespace ::dbj::util;
			using inside_1_and_max =
				insider<size_t, 1,
				::dbj::buf::max_length
				, insider_error_code_throw_policy
				>;
		}

		struct arrbuf final
		{
			using value_type = char;
			using type = std::unique_ptr<char[]>;
			using ref_type = std::reference_wrapper< type >;
		};

		using smarty = typename arrbuf::type;
		using smarty_ref = typename arrbuf::ref_type;

		// always use this function to make fresh smarty-es!
		DBJ_NODISCARD  inline auto smart(inside_1_and_max size_) noexcept
			-> typename smarty
		{
			return std::make_unique<char[]>(size_ + 1);
		}

		DBJ_NODISCARD  inline auto length(smarty_ref buf_ref_)
			noexcept -> inside_1_and_max
		{
			return std::strlen((buf_ref_.get()).get());
		}

		/*
		make from array of char's
		*/
		template<size_t N>
		DBJ_NODISCARD  inline auto smart(const char(&charr)[N])
			noexcept -> smarty
		{
			assert(N > 0);
			smarty sp_ = smart (N);
			void * rez_ = ::memcpy(sp_.get(), charr, N);
			assert(rez_);
			return sp_; // the move
		}

		/* make smarty from string_view */
		DBJ_NODISCARD  inline 
			auto	smart
		( string_view sv_) noexcept
			-> smarty
		{
			assert(!sv_.empty());
			// is this naive?
			// should I use strnlen() ?
			inside_1_and_max N = sv_.size();
			smarty sp_ = smart (N);
			void * rez_ = ::memcpy(sp_.get(), sv_.data(), N);
			assert(rez_);
			return sp_;
		}

		/*
		make smarty<C> from smarty<C>
		this is clever since we do not deal 
		with C *, but... can we rely on the the caller
		making a properly zero terminated string in there
		so that strlen will work?
		*/
		DBJ_NODISCARD  inline auto
			smart (smarty_ref buf_ref_) noexcept -> smarty
		{
			smarty & from_ = buf_ref_;
			assert(from_);
			inside_1_and_max N = ::dbj::buf::length (buf_ref_);
			assert(N > 1);
			smarty sp_ = ::dbj::buf::smart (N);
			assert(sp_);
			void * rez_ = ::memcpy(sp_.get(), buf_ref_.get().get(), N);
			assert(rez_);
			return sp_;
		}

		/*
		assign array to instance of unique_ptr<T[]>
		note: "any" type will do as long as std::unique_ptr
		will accept it
		*/
		template<typename C, size_t N>
		inline auto
			assign
			(
				std::unique_ptr<C[]> & sp_,
				const C(&arr)[N]
			) noexcept
			-> std::unique_ptr<C[]> &
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

		// dangerously low, bit it works
		inline void
			fill(smarty_ref bref_, size_t N = 0, char val_ = char{}) noexcept
		{
			smarty & buff_ = bref_;
			if (!N) N = ::dbj::buf::length(bref_);
			void *p = (void *)buff_.get();
			::std::memset(p, val_, N);
		}

		/*
		--------------------------------------------------------------------------------
		Above is for fast-and-dangerous code and bellow is a comfortable runtime 
		char buffer.
		Slower than naked uniq_ptr but still approx 2x to 3x faster vs vector<char>
		I have the code for measuring the difference,
		in case you are hotly against *not* using vector<char>.
		Generaly the smaller the buffer the faster it is vs vector<char>.
		For *normal* buffer sizes (posix BUFSIZ or multiplies of it)
		using this type has a lot in favour.
		Also this class works in code where no exceptions are used
		please do let me know if problems there
		*/
		struct char_buffer final {

			using type = char_buffer;
			using pointer = smarty;
			using value_type = char;
			using iterator = value_type * ;
			using reference_type = type & ;

			explicit char_buffer(inside_1_and_max size) noexcept { this->reset(size); }

			char_buffer(const char_buffer & another_) noexcept
			{
				this->reset(another_.size_);
				(void)::memcpy(data(), another_.data(), this->size());
			}
			char_buffer & operator =(const char_buffer & another_) noexcept
			{
				if (&another_ != this) {
					this->reset(another_.size_);
					(void)::memcpy(data(), another_.data(), this->size());
				}
				return *this;
			}

			template<size_t N>
			char_buffer(char(&charr)[N]) noexcept
			{
				this->size_ = N;
				this->data_ = smart (charr);
			}

			void reset(inside_1_and_max new_size_) const {
				data_.release();
				this->size_ = new_size_;
				this->data_ = smart (new_size_);
			}

			char & operator [] (inside_1_and_max idx_)
			{
				return data_[idx_]; //  std::unique_ptr<T[]> has this operator
			}

			iterator data() const noexcept { return data_.get(); }

			size_t const & size() const noexcept { return size_; }

			value_type ** const address() const noexcept {
				auto p = std::addressof(data_[0]);
				return std::addressof(p);
			}
#pragma region std lib conformance

			iterator begin() noexcept { return data_.get(); }
			iterator end()   noexcept { return data_.get() + size_; }
			const iterator begin() const noexcept { return data_.get(); }
			const iterator end()   const noexcept { return data_.get() + size_; }

			void fill(char val_ = char{}) const noexcept
			{
				::dbj::buf::fill(
					std::reference_wrapper<pointer>(data_),
					size(),
					val_
				);
			}
#pragma endregion 

			// to avoid "never releasing smart pointer" syndrome
			// we wil ban creation of this class on the heap
			void* operator new(std::size_t sz) = delete;
			void* operator new[](std::size_t sz) = delete;
			void  operator delete(void* ptr, std::size_t sz) = delete;
			void  operator delete[](void* ptr, std::size_t sz) = delete;
		private:
			mutable inside_1_and_max  size_;
			mutable	pointer data_{}; // size == 0
#pragma region char_buffer friends

			friend std::string to_string(const reference_type from_) noexcept {
				// this will not copy the whole buffer
				return { from_.data() };
			}

			friend std::vector<char> to_vector(const reference_type from_) noexcept {
				// this will not copy the whole buffer
				std::string str_(from_.data());
				return { str_.begin(), str_.end() };
			}

			/*
			copy this one to the buffer of the same size
			return error_code(std::errc::invalid_argument)
			on different sizes
			return OK error_code on no error
			*/
			DBJ_NODISCARD  friend
				std::error_code copy_to(const reference_type from_, reference_type target_)
				noexcept /* beware: std::copy may throw bad_alloc! */
			{
				if (from_.size() != target_.size())
					return std::make_error_code(std::errc::invalid_argument);

				std::copy(from_.data_.get(), from_.data_.get() + from_.size(),
					target_.data_.get());

				return std::error_code{}; // OK
			}

#pragma endregion // char_buffer friend utilities
		}; // char_buffer

	} // buf
} // dbj
