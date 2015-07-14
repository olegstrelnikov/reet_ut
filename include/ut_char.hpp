/*
 * ut_char.hpp
 *
 *  Created on: Jul 14, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef INCLUDE_UT_CHAR_HPP_
#define INCLUDE_UT_CHAR_HPP_

#ifndef UT_NAMESPACE
#define UT_NAMESPACE ut
#endif

namespace UT_NAMESPACE {

template<typename T> class Is {
public:
    static constexpr bool Char = false;
};

template<> class Is<char> {
public:
    static constexpr bool Char = true;
};

template<> class Is<signed char> {
public:
    static constexpr bool Char = true;
};

template<> class Is<unsigned char> {
public:
    static constexpr bool Char = true;
};

template<> class Is<wchar_t> {
public:
    static constexpr bool Char = true;
};

template<> class Is<char16_t> {
public:
    static constexpr bool Char = true;
};

template<> class Is<char32_t> {
public:
    static constexpr bool Char = true;
};

template<typename T> class IsPointer {
public:
    static constexpr bool ToChar = false;
};

template<> class IsPointer<char*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<signed char*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<unsigned char*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<const char*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<const signed char*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<const unsigned char*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<const wchar_t*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<const char16_t*> {
public:
    static constexpr bool ToChar = true;
};

template<> class IsPointer<const char32_t*> {
public:
    static constexpr bool ToChar = true;
};

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_CHAR_HPP_ */
