#pragma once

namespace std
{
/*
	template <class T>
	struct remove_reference { typedef T type; };

	template <class T>
	struct remove_reference<T&> { typedef T type; };

	template <class T>
	struct remove_reference<T&&> { typedef T type; };
*/
	// https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.4/a01396.html
	#define _DEFINE_SPEC_0_HELPER                          \
		template<>

	#define _DEFINE_SPEC_1_HELPER                          \
		template<typename _Tp>

	#define _DEFINE_SPEC_2_HELPER                          \
		template<typename _Tp, typename _Cp>

	#define _DEFINE_SPEC(_Order, _Trait, _Type, _Value)    \
		_DEFINE_SPEC_##_Order##_HELPER                     \
			struct _Trait<_Type>                           \
			: public integral_constant<bool, _Value> { };

	// helper classes [4.3].

	/// integral_constant
	template<typename _Tp, _Tp __v>
		struct integral_constant
		{
			static const _Tp                      value = __v;
			typedef _Tp                           value_type;
			typedef integral_constant<_Tp, __v>   type;
		};

	/// typedef for true_type
	typedef integral_constant<bool, true>     true_type;
	/// typedef for false_type
	typedef integral_constant<bool, false>    false_type;
	template<typename _Tp, _Tp __v>
		const _Tp integral_constant<_Tp, __v>::value;

	/// remove_cv
	template<typename>
		struct remove_cv;
	template<typename>
		struct __is_void_helper
		: public false_type { };
	_DEFINE_SPEC(0, __is_void_helper, void, true)
	// primary type categories [4.5.1].
	/// is_void
	template<typename _Tp>
		struct is_void
		: public integral_constant<bool, (__is_void_helper<typename
		                remove_cv<_Tp>::type>::value)>
		{ };

	// https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-html-USERS-4.4/a01394.html
	/// is_lvalue_reference
	template<typename>
		struct is_lvalue_reference
		: public false_type { };
	template<typename _Tp>
		struct is_lvalue_reference<_Tp&>
		: public true_type { };
	/// is_rvalue_reference
	template<typename>
		struct is_rvalue_reference
		: public false_type { };
	template<typename _Tp>
		struct is_rvalue_reference<_Tp&&>
		: public true_type { };
	// Secondary classification traits.
	/// is_reference
	template<typename _Tp>
		struct is_reference
		: public integral_constant<bool, (is_lvalue_reference<_Tp>::value
						|| is_rvalue_reference<_Tp>::value)>
		{ };
	/// remove_reference
	template<typename _Tp>
		struct remove_reference
		{ typedef _Tp   type; };
	template<typename _Tp>
		struct remove_reference<_Tp&>
		{ typedef _Tp   type; };
	template<typename _Tp>
		struct remove_reference<_Tp&&>
		{ typedef _Tp   type; };
	template<typename _Tp,
			bool = !is_reference<_Tp>::value && !is_void<_Tp>::value,
			bool = is_rvalue_reference<_Tp>::value>
		struct __add_lvalue_reference_helper
		{ typedef _Tp   type; };
	template<typename _Tp>
		struct __add_lvalue_reference_helper<_Tp, true, false>
		{ typedef _Tp&   type; };
	template<typename _Tp>
		struct __add_lvalue_reference_helper<_Tp, false, true>
		{ typedef typename remove_reference<_Tp>::type&   type; };


/*
	// https://en.cppreference.com/w/cpp/types/enable_if
	template<bool B, class T = void>
	struct enable_if {};

	template<class T>
	struct enable_if<true, T> { typedef T type; };
*/
	// Define a nested type if some predicate holds.
	// Primary template.
	/// enable_if
	template<bool, typename _Tp = void>
		struct enable_if
		{ };
	// Partial specialization for true.
	template<typename _Tp>
		struct enable_if<true, _Tp>
		{ typedef _Tp type; };

/*
	// https://en.cppreference.com/w/cpp/types/is_same
	template<class T, class U>
	struct is_same : std::__false_type {};

	template<class T>
	struct is_same<T, T> : std::__true_type {};
*/

	// https://gcc.gnu.org/onlinedocs/gcc-8.1.0/libstdc++/api/a03904.html
	// https://gcc.gnu.org/onlinedocs/gcc-8.1.0/libstdc++/api/a00167_source.html
	/// is_same
   template<typename, typename>
	 struct is_same
	 : public false_type { };

   template<typename _Tp>
	 struct is_same<_Tp, _Tp>
	 : public true_type { };

}
