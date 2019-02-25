#ifndef CLI_EXPERIMENTAL17_TYPE_TRAITS_H
#define CLI_EXPERIMENTAL17_TYPE_TRAITS_H

#include <type_traits>

namespace cli
{
namespace experimental
{
	template<class...>
	using void_t = void;

	template<typename B1_, typename B2_>
	struct or_ : public std::conditional<B1_::value, B1_, B2_>::type
	{};

	// __is_invocable (std::is_invocable for C++11)

	template<typename Result_, typename Ret_, typename = void>
	struct is_invocable_impl_ : std::false_type
	{};

	template<typename Result_, typename Ret_>
	struct is_invocable_impl_<Result_, Ret_, void_t<typename Result_::type>>
			: or_<std::is_void<Ret_>, std::is_convertible<typename Result_::type, Ret_>>::type
	{};


/// std::is_invocable
	template<typename Fn_, typename... ArgTypes_>
	struct is_invocable : is_invocable_impl_<std::result_of<Fn_(ArgTypes_...)>, void>::type
	{};

/// std::is_invocable_r
	template<typename Ret_, typename Fn_, typename... ArgTypes_>
	struct is_invocable_r
			: is_invocable_impl_<std::result_of<Fn_(ArgTypes_...)>, Ret_>::type
	{};
}
}

#endif
