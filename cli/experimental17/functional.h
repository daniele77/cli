#ifndef CLI_EXPERIMENTAL17_FUNCTIONAL_H
#define CLI_EXPERIMENTAL17_FUNCTIONAL_H

#include <functional>
#include "cli/experimental17/type_traits.h"

namespace cli
{
namespace experimental
{
	template<typename Tp_>
	struct success_type_
	{
		typedef Tp_ type;
	};

	struct failure_type_
	{
	};

	struct invoke_memfun_ref_
	{
	};
	struct invoke_memfun_deref_
	{
	};
	struct invoke_memobj_ref_
	{
	};
	struct invoke_memobj_deref_
	{
	};
	struct invoke_other_
	{
	};

	// Associate a tag type with a specialization of success_type_.
	template<typename Tp_, typename Tag_>
	struct result_of_success_ : success_type_<Tp_>
	{
		using invoke_type_ = Tag_;
	};

	// [func.require] paragraph 1 bullet 1:
	struct result_of_memfun_ref_impl_
	{
		template<typename Fp_, typename Tp1_, typename... Args_>
		static result_of_success_<decltype(
		(std::declval<Tp1_>().*std::declval<Fp_>())(std::declval<Args_>()...)
		), invoke_memfun_ref_> S_test_(int);

		template<typename...>
		static failure_type_ S_test_(...);
	};

	template<typename MemPtr_, typename Arg_, typename... Args_>
	struct result_of_memfun_ref_
			: private result_of_memfun_ref_impl_
	{
		typedef decltype(S_test_<MemPtr_, Arg_, Args_...>(0)) type;
	};

	// [func.require] paragraph 1 bullet 2:
	struct result_of_memfun_deref_impl_
	{
		template<typename Fp_, typename Tp1_, typename... Args_>
		static result_of_success_<decltype(
		((*std::declval<Tp1_>()).*std::declval<Fp_>())(std::declval<Args_>()...)
		), invoke_memfun_deref_> S_test_(int);

		template<typename...>
		static failure_type_ S_test_(...);
	};

	template<typename MemPtr_, typename Arg_, typename... Args_>
	struct result_of_memfun_deref_
			: private result_of_memfun_deref_impl_
	{
		typedef decltype(S_test_<MemPtr_, Arg_, Args_...>(0)) type;
	};

	// [func.require] paragraph 1 bullet 3:
	struct result_of_memobj_ref_impl_
	{
		template<typename Fp_, typename Tp1_>
		static result_of_success_<decltype(
		std::declval<Tp1_>().*std::declval<Fp_>()
		), invoke_memobj_ref_> S_test_(int);

		template<typename, typename>
		static failure_type_ S_test_(...);
	};

	template<typename MemPtr_, typename Arg_>
	struct result_of_memobj_ref_
			: private result_of_memobj_ref_impl_
	{
		typedef decltype(S_test_<MemPtr_, Arg_>(0)) type;
	};

	// [func.require] paragraph 1 bullet 4:
	struct result_of_memobj_deref_impl_
	{
		template<typename Fp_, typename Tp1_>
		static result_of_success_<decltype(
		(*std::declval<Tp1_>()).*std::declval<Fp_>()
		), invoke_memobj_deref_> S_test_(int);

		template<typename, typename>
		static failure_type_ S_test_(...);
	};

	template<typename MemPtr_, typename Arg_>
	struct result_of_memobj_deref_
			: private result_of_memobj_deref_impl_
	{
		typedef decltype(S_test_<MemPtr_, Arg_>(0)) type;
	};

	template<typename MemPtr_, typename Arg_>
	struct result_of_memobj_;

	template<typename Res_, typename Class_, typename Arg_>
	struct result_of_memobj_<Res_ Class_::*, Arg_>
	{
		typedef typename std::remove_cv<typename std::remove_reference<Arg_>::type>::type Argval_;
		typedef Res_ Class_::* MemPtr_;
		typedef typename std::conditional<or_<std::is_same<Argval_, Class_>,
				std::is_base_of<Class_, Argval_>>::value,
				result_of_memobj_ref_<MemPtr_, Arg_>,
				result_of_memobj_deref_<MemPtr_, Arg_>
		>::type::type type;
	};

	template<typename MemPtr_, typename Arg_, typename... Args_>
	struct result_of_memfun_;

	template<typename Res_, typename Class_, typename Arg_, typename... Args_>
	struct result_of_memfun_<Res_ Class_::*, Arg_, Args_...>
	{
		typedef typename std::remove_cv<typename std::remove_reference<
				Arg_>::type>::type Argval_;
		typedef Res_ Class_::* MemPtr_;
		typedef typename std::conditional<or_<std::is_same<Argval_, Class_>,
				std::is_base_of<Class_, Argval_>>::value,
				result_of_memfun_ref_<MemPtr_, Arg_, Args_...>,
				result_of_memfun_deref_<MemPtr_, Arg_, Args_...>
		>::type::type type;
	};

	// Used by result_of, invoke etc. to unwrap a reference_wrapper.
	template<typename Tp_, typename Up_ = typename std::decay<Tp_>::type>
	struct inv_unwrap_
	{
		using type = Tp_;
	};

	template<typename Tp_, typename Up_>
	struct inv_unwrap_<Tp_, std::reference_wrapper<Up_>>
	{
		using type = Up_&;
	};

	// Used by invoke_impl_ instead of std::forward<Tp_> so that a
	// reference_wrapper is converted to an lvalue-reference.
	template<typename Tp_, typename Up_ = typename inv_unwrap_<Tp_>::type>
	constexpr Up_&&
	invfwd_(typename std::remove_reference<Tp_>::type& t_) noexcept
	{ return static_cast<Up_&&>(t_); }

	template<typename Res_, typename Fn_, typename... Args_>
	constexpr Res_
	invoke_impl_(invoke_other_, Fn_&& f_, Args_&& ... args_)
	{ return std::forward<Fn_>(f_)(std::forward<Args_>(args_)...); }

	template<typename Res_, typename MemFun_, typename Tp_, typename... Args_>
	constexpr Res_
	invoke_impl_(invoke_memfun_ref_, MemFun_&& f_, Tp_&& t_,
	             Args_&& ... args_)
	{ return (invfwd_<Tp_>(t_).*f_)(std::forward<Args_>(args_)...); }

	template<typename Res_, typename MemFun_, typename Tp_, typename... Args_>
	constexpr Res_
	invoke_impl_(invoke_memfun_deref_, MemFun_&& f_, Tp_&& t_,
	             Args_&& ... args_)
	{ return ((*std::forward<Tp_>(t_)).*f_)(std::forward<Args_>(args_)...); }

	template<typename Res_, typename MemPtr_, typename Tp_>
	constexpr Res_
	invoke_impl_(invoke_memobj_ref_, MemPtr_&& f_, Tp_&& t_)
	{ return invfwd_<Tp_>(t_).*f_; }

	template<typename Res_, typename MemPtr_, typename Tp_>
	constexpr Res_
	invoke_impl_(invoke_memobj_deref_, MemPtr_&& f_, Tp_&& t_)
	{ return (*std::forward<Tp_>(t_)).*f_; }

	template<bool, bool, typename Functor_, typename... ArgTypes_>
	struct result_of_impl_
	{
		typedef failure_type_ type;
	};

	template<typename MemPtr_, typename Arg_>
	struct result_of_impl_<true, false, MemPtr_, Arg_>
			: public result_of_memobj_<typename std::decay<MemPtr_>::type,
					typename inv_unwrap_<Arg_>::type>
	{
	};

	template<typename MemPtr_, typename Arg_, typename... Args_>
	struct result_of_impl_<false, true, MemPtr_, Arg_, Args_...>
			: public result_of_memfun_<typename std::decay<MemPtr_>::type,
					typename inv_unwrap_<Arg_>::type, Args_...>
	{
	};

	// [func.require] paragraph 1 bullet 5:
	struct result_of_other_impl_
	{
		template<typename Fn_, typename... Args_>
		static result_of_success_<decltype(
		std::declval<Fn_>()(std::declval<Args_>()...)
		), invoke_other_> S_test_(int);

		template<typename...>
		static failure_type_ S_test_(...);
	};

	template<typename Functor_, typename... ArgTypes_>
	struct result_of_impl_<false, false, Functor_, ArgTypes_...>
			: private result_of_other_impl_
	{
		typedef decltype(S_test_<Functor_, ArgTypes_...>(0)) type;
	};

	// invoke_result_ (std::invoke_result for C++11)
	template<typename Functor_, typename... ArgTypes_>
	struct invoke_result_
			: public result_of_impl_<
					std::is_member_object_pointer<
							typename std::remove_reference<Functor_>::type
					>::value,
					std::is_member_function_pointer<
							typename std::remove_reference<Functor_>::type
					>::value,
					Functor_, ArgTypes_...
			>::type
	{
	};


	/// Invoke a callable object.
	template<typename Callable_, typename... Args_>
	constexpr typename std::result_of<Callable_(Args_...)>::type invoke_(Callable_&& fn_, Args_&& ... args_)
	{
		using result_ = invoke_result_<Callable_, Args_...>;
		using type_ = typename result_::type;
		using tag_ = typename result_::invoke_type_;

		return invoke_impl_<type_>(tag_{}, std::forward<Callable_>(fn_), std::forward<Args_>(args_)...);
	}

	/// Invoke a callable object.
	template<typename Callable_, typename... Args_>
	inline std::result_of_t<Callable_(Args_...)> invoke(Callable_&& fn_, Args_&& ... args_)
	{
		return invoke_(std::forward<Callable_>(fn_), std::forward<Args_>(args_)...);
	}
}
}

#endif
