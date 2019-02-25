#ifndef CLI_EXPERIMENTAL17_TUPLE_H
#define CLI_EXPERIMENTAL17_TUPLE_H

#include <tuple>
#include <utility>

#include "cli/experimental17/functional.h"
#include "cli/experimental17/type_traits.h"

namespace cli
{
namespace experimental
{
	namespace detail
	{
		template<class F, class Tuple, std::size_t... I>
		constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
		{ return GC::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...); }
	}

	template<class F, class Tuple>
	constexpr decltype(auto) apply(F&& f, Tuple&& t)
	{
		return detail::apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
		                          std::make_index_sequence<std::tuple_size<std::decay_t<Tuple >>::value> {});
	}
}
}

#endif
