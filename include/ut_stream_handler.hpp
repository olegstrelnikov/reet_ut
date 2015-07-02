/*
 * ut_stream_handler.hpp
 *
 *  Created on: Jun 18, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef INCLUDE_UT_STREAM_HANDLER_HPP_
#define INCLUDE_UT_STREAM_HANDLER_HPP_

#include "ut_stream_serializer.hpp"

#ifndef UT_NAMESPACE
#define UT_NAMESPACE ut
#endif

namespace UT_NAMESPACE {

	template<typename E> class Stream {
	public:
		typedef E Exception;
		template<typename Out> static void Message(Exception const& e, Out o) {
			StreamSerializer::serialize(e, o);
		}
	}; //class Stream

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_STREAM_HANDLER_HPP_ */
