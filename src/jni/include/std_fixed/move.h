#pragma once

#include "type_traits.h"

namespace std
{
	// https://en.cppreference.com/w/cpp/utility/forward
	// https://gcc.gnu.org/onlinedocs/gcc-4.8.5/libstdc++/api/a01417.html
/*
	template<typename T>
	constexpr T && forward(typename std::remove_reference<T>::type & __t) noexcept
	{
		return static_cast<T &&>(__t);
	}
*/

	// https://gcc.gnu.org/onlinedocs/gcc-8.1.0/libstdc++/api/a00422_source.html#l00074
	/**
	 *  @brief  Forward an lvalue.
	 *  @return The parameter cast to the specified type.
	 *
	 *  This function is used to implement "perfect forwarding".
	 */
	template<typename _Tp>
	   constexpr _Tp&&
	   forward(typename std::remove_reference<_Tp>::type& __t) noexcept
	   { return static_cast<_Tp&&>(__t); }

	// https://gcc.gnu.org/onlinedocs/gcc-8.1.0/libstdc++/api/a00422_source.html#l00085
	/**
	 *  @brief  Forward an rvalue.
	 *  @return The parameter cast to the specified type.
	 *
	 *  This function is used to implement "perfect forwarding".
	 */
	template<typename _Tp>
	   constexpr _Tp&&
	   forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
	   {
		static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument"
					  " substituting _Tp is an lvalue reference type");
		return static_cast<_Tp&&>(__t);
	   }
}
