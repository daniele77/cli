#pragma once

#include "LICENCE.h"
#include <system_error>

#if __cplusplus < 201703
#include "string_veiw.h"
#endif

#if ! (__cplusplus < 201703)
#define DBJ_NODISCARD [[nodiscard]] 
#else
#define DBJ_NODISCARD 
#endif

namespace dbj {
	namespace util {

#if __cplusplus < 201703
		using bpstd::string_view; // back-port std project
#endif
		/*
		no more repeated asserts and checks == cleaner/safer/faster code,
		example:

		using inside_42_and_max
		= insider<size_t, 42, BUFSIZ >;

		no checks inside at()
		and it is transparent checks to the client code

		extern int & at (inside_42_and_max idx_);

		at(43) ; // OK
		at(41) ; // fails at runtime
		*/

		template<typename T, T L, T H >	struct insider_silent_policy;
		template<typename T, T L, T H >	struct insider_exit_policy;
		template<typename T, T L, T H >	struct insider_error_code_throw_policy;

		template<typename T, T L, T H,
			template<typename T, T L, T H > typename P = insider_silent_policy >
		struct insider final
		{
#if ! (__cplusplus < 201703)
			// otherwise insider itself will not be
			static_assert(std::is_trivial_v<T>  && std::is_move_constructible_v<T>);
#endif

			using policy = typename P<T, L, H>;
			using value_type = T;
			using type = insider;

			constexpr static const T low = L;
			constexpr static const T high = H;

			insider(const T & newsz = L) noexcept
				: value(newsz) {
				valid_ = policy::check(value);
			}
			operator T & ()    const noexcept { return value; }
			T &  operator () () const noexcept { return value; }

			/*	simple check for clients is the policy result	*/
			bool valid() const noexcept { return valid_; }
		private:
			mutable T value{};
			mutable bool valid_{};
		};

		/*
		this is the default policy and it is not a good policy
		is just returns true/false on check's
		*/
		template<typename T, T L, T H >
		struct insider_silent_policy final
		{
			static bool check(T const & v_)
			{
				if (v_ >= L)
					if (v_ <= H)
						return true;
				return false;
			}
		};

		/*
		this is a brutal policy, it exits the app
		if value is not in range
		*/
		template<typename T, T L, T H >
		struct insider_exit_policy final
		{
			static bool check(T const & v_)
			{
				assert(v_ >= L); // kick's the bucket
				assert(v_ <= H); // at runtime
				return true;
			}
		};

		/*
		this is my favourite
		it throws EINVAL on error not an std::exception
		*/
		template<typename T, T L, T H >
		struct insider_error_code_throw_policy final
		{
			static bool check(T const & v_)
			{
				if (v_ >= L)
					if (v_ <= H)
						return true;
				throw std::make_error_code(std::errc::invalid_argument);
			}
		};

	} // util
} // dbj
