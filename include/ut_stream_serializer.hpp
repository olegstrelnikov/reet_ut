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

	template<typename E> class Stream {
	public:
		typedef E Exception;
		template<typename Out> static void Message(Exception const& e, Out o) {
			std::ostringstream oss;
			oss << e;
			std::string s = oss.str();
			std::copy(std::begin(s), std::end(s), o);
		}
	}; //class Stream

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_STREAM_SERIALIZER_HPP_ */
