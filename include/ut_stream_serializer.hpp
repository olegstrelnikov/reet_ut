/*
 * ut_stream_serializer.hpp
 *
 *  Created on: Jul 1, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef INCLUDE_UT_STREAM_SERIALIZER_HPP_
#define INCLUDE_UT_STREAM_SERIALIZER_HPP_

#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

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


class StreamSerializer {
private:
	template<typename T, bool IsPtrToChar = IsPointer<T>::ToChar> class Character {
	public:
        static void serialize(std::ostringstream& oss, T const& t) {
            oss << t;
        }
	};
	template<typename T> class Character<T, true> {
	public:
        static void serialize(std::ostringstream& oss, const T t) {
        	if (nullptr == t) {
        		oss << "nullptr";
        	} else {
        		oss << '\"' << t << '\"';
        	}
        }
	};
    template<typename T, bool IsChar = Is<T>::Char> class Value {
    public:
        static void serialize(std::ostringstream& oss, std::size_t const N, T const *t) {
            oss << "{ ";
            if (N > 0) {
                StreamSerializer::serialize(t[0], std::ostream_iterator<char>(oss));
                for (std::size_t i = 1; i < N; ++i) {
                	oss << ", ";
                    StreamSerializer::serialize(t[i], std::ostream_iterator<char>(oss));
                }
            }
            oss << " }";
        }
        static void serialize(std::ostringstream& oss, T const& t) {
            Character<T>::serialize(oss, t);
        }
    };
    template<typename T> class Value<T, true> {
    public:
        static void serialize(std::ostringstream& oss, std::size_t const N, T const* t) {
            oss << "\"";
            if (N > 0)
            {
                for (std::size_t i = 0; i < N - 1; ++i) {
                    Value<T, false>::serialize(oss, t[i]);
                }
                if (t[N - 1]) {
                    Value<T, false>::serialize(oss, t[N - 1]);
                }
            }
            oss << "\"";
        }
        static void serialize(std::ostringstream& oss, T const& t) {
            oss << '\'' << t << '\'';
        }
    };
public:
    template<typename T, typename Out> static void serialize(T const& t, Out o) {
        std::ostringstream oss;
        Value<T>::serialize(oss, t);
        std::string s = oss.str();
        std::copy(s.begin(), s.end(), o);
    }

    template<typename T, std::size_t N, typename Out> static void serialize(T const (&t)[N], Out o) {
        std::ostringstream oss;
        Value<T>::serialize(oss, N, t);
        std::string s = oss.str();
        std::copy(s.begin(), s.end(), o);
    }
}; //class StreamSerializer

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_STREAM_SERIALIZER_HPP_ */
