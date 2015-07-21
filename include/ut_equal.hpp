/*
 * ut_equal.hpp
 *
 *  Created on: Jul 14, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef INCLUDE_UT_EQUAL_HPP_
#define INCLUDE_UT_EQUAL_HPP_

#include <algorithm> //std::equal
#include <initializer_list>
#include <cstddef> //std::size_t

#include "ut_namespace.hpp"
#include "ut_char.hpp"

namespace UT_NAMESPACE {

struct EqualResult {
    enum Result {Equal, SizeNotEqual, ElementNotEqual} const result;
    std::size_t const index;
    EqualResult(Result r, std::size_t i = std::size_t()) : result(r), index(i) {}
};

template<typename T1, typename T2> class EqualFunction {
public:
    typedef bool (*Type)(T1 const&, T2 const&);
};

template<typename T1, typename T2> bool basic_equal(T1 const& t1, T2 const& t2) {
    return t1 == t2;
}

template<typename T1, typename T2> bool equal(T1 const& t1, T2 const& t2);

template<typename I, typename C> bool cstring_equal(I first, I last, C const* p) {
    for (; first != last && !equal<C, C>(*p, C()) && equal<decltype(*first), C>(*first, *p); ++first, ++p);
    return (first != last && equal<decltype(*first), C>(*first, C()) && ++first == last && equal<C, C>(*p, C()));
}

template<typename C1, typename C2> bool cstring_equal(C1 const* p1, C2 const* p2) {
    for (; !equal(*p1, C1()) && !equal(*p2, C2()) && equal(*p1, *p2); ++p1, ++p2);
    return (equal(*p1, C1()) && equal(*p2, C2()));
}

template<typename T> constexpr bool IsPointerToChar() {
    return IsPointer<T>::ToChar;
}

template<typename I1, typename T2, bool Is2PointerToChar = IsPointerToChar<T2>()>
class Sequence {
public:
    static bool equal(I1 first, I1 last, std::size_t const N, T2 const& t2) {
        return (1 == N) ? UT_NAMESPACE::equal(*first, t2) : (N == t2.size()) && sequence_equal(first, last, t2.begin());
    }
};

template<typename I1, typename T2> class Sequence<I1, T2, true> {
public:
    static bool equal(I1 first, I1 last, std::size_t const, T2 const& t2) {
        return cstring_equal(first, last, t2);
    }
};

template<typename T> class Dereference;
template<typename T> class Dereference<T*> {
public:
    typedef T Type;
};

enum TypeSpecial {PointerToChar, Container, Other};

template<typename T> class ContainerConcept {
public:
    static constexpr bool satisfies = false;
};

template<typename T, typename A> class ContainerConcept<std::vector<T, A>> {
public:
    static constexpr bool satisfies = true;
};

template<typename T> constexpr bool IsContainer() {
    return ContainerConcept<T>::satisfies;
}

template<typename T> constexpr TypeSpecial Specialize() {
    return IsPointerToChar<T>() ? PointerToChar : IsContainer<T>() ? Container : Other;
}

template<typename T1, TypeSpecial T1Is = Specialize<T1>()> class Equal {
public:
	template<typename T2, bool R = (T1Is == Specialize<T2>())> class To {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return basic_equal(t1, t2);
	    }
	};
	template<typename T2> class To<T2, false> {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return Equal<T2>::template To<T1>::equal(t2, t1);
	    }
	};
};

template<typename T1> class Equal<T1, Container> {
public:
	template<typename T2, TypeSpecial T2Is = Specialize<T2>()> class To {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return Sequence<decltype(t1.begin()), T2>::equal(t1.begin(), t1.end(), t1.size(), t2);
	    }
	};
	template<typename T2> class To<T2, PointerToChar> {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return cstring_equal(t1.begin(), t1.end(), t2);
	    }
	};
};

template<typename T1> class Equal<T1, PointerToChar> {
public:
	template<typename T2, TypeSpecial T2Is = Specialize<T2>()> class To {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return UT_NAMESPACE::equal(t1, t2[0]) && UT_NAMESPACE::equal(Dereference<T2>::Type(), t2[1]);
	    }
	};
	template<typename T2> class To<T2, PointerToChar> {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return cstring_equal(t1, t2);
	    }
	};
	template<typename T2> class To<T2, Container> {
	public:
	    static bool equal(T1 const& t1, T2 const& t2) {
	        return Equal<T2>::template To<T1>::equal(t2, t1);
	    }
	};
};

// 12-21 - general cases
template<typename T1, typename T2> bool equal(T1 const& t1, T2 const& t2) {
    return Equal<T1>::template To<T2>::equal(t1, t2);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// array special cases:
// 1 - array-array
template<typename T1, typename T2, std::size_t N1, std::size_t N2> bool equal(T1 const (&t1)[N1], T2 const (&t2)[N2]);
template<typename T1, typename T2, std::size_t N1, std::size_t N2, std::size_t M1, std::size_t M2>
	bool equal(T1 const (&t1)[N1][M1], T2 const (&t2)[N2][M2]);
template<typename T1, typename T2, std::size_t N> bool equal(T1 const (&t1)[N], T2 const (&t2)[N]) {
    return std::equal<T1 const*, T2 const*, typename EqualFunction<T1, T2>::Type>(t1, t1 + N, t2, equal<T1, T2>);
}
template<typename T1, typename T2, std::size_t N, std::size_t M> bool equal(T1 const (&t1)[N][M], T2 const (&t2)[N][M]) {
	for (std::size_t i = 0; i < N; ++i) { //todo: is it possible to replace the loop with std::equal() ?
		if (!equal(t1[i], t2[i])) {
			return false;
		}
	}
    return true;
}

// 2 - array-initializer_list
template<typename T1, typename T2, std::size_t N1> bool equal(T1 const (&t1)[N1], std::initializer_list<T2> t2) {
    return (N1 == t2.size()) && std::equal<T1 const*, typename std::initializer_list<T2>::const_iterator, typename EqualFunction<T1, T2>::Type>(t1, t1 + N1, t2.begin(), equal<T1, T2>);
}
template<typename T1, typename T2, std::size_t N1, std::size_t M1> bool equal(T1 const (&t1)[N1][M1], std::initializer_list<T2> t2) {
	if (N1 != t2.size()) {
		return false;
	}
	auto it = t2.begin();
	for (std::size_t i = 0; i < N1; ++i, ++it) { //todo: is it possible to replace the loop with std::equal() ?
		if (!equal(t1[i], *it)) {
			return false;
		}
	}
    return true;
}

// 2' - initializer_list-array
template<typename T1, typename T2, std::size_t N2> bool equal(std::initializer_list<T1> t1, T2 const (&t2)[N2]) {
    return equal(t2, t1);
}
template<typename T1, typename T2, std::size_t N2, std::size_t M2> bool equal(std::initializer_list<T1> t1, T2 const (&t2)[N2][M2]) {
    return equal(t2, t1);
}

// 3-6 - array-container, string, other
template<typename T1, typename T2, std::size_t N1> bool equal(T1 const (&t1)[N1], T2 const &t2) {
    return Sequence<T1 const*, T2>::equal(t1, t1 + N1, N1, t2);
}

// 3-6'
template<typename T1, typename T2, std::size_t N2> bool equal(T1 const& t1, T2 const (&t2)[N2]) {
    return equal<T2, T1, N2>(t2, t1);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// initializer_list special cases:
// 7 - initializer_list-initializer_list
template<typename T1, typename T2> bool equal(std::initializer_list<T1> t1, std::initializer_list<T2> t2) {
    return (t1.size() == t2.size()) && std::equal<typename std::initializer_list<T1>::const_iterator, typename std::initializer_list<T2>::const_iterator, typename EqualFunction<T1, T2>::Type>(t1.begin(), t1.end(), t2.begin(), equal<T1, T2>);
}

// 8 - initializer_list-container, string, other
template<typename T1, typename T2> bool equal(std::initializer_list<T1> t1, T2 const& t2) {
    return Sequence<decltype(t1.begin), T2>::equal(t1.begin(), t1.end(), t1.size(), t2);
}

// 8'
template<typename T1, typename T2> bool equal(T1 const& t1, std::initializer_list<T2> t2) {
    return equal(t2, t1);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_EQUAL_HPP_ */
