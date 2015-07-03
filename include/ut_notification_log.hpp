/*
 * ut_notification_log.hpp
 *
 *  Created on: Jul 3, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef INCLUDE_UT_NOTIFICATION_LOG_HPP_
#define INCLUDE_UT_NOTIFICATION_LOG_HPP_

#include "ut.hpp"

#ifndef UT_NAMESPACE
#define UT_NAMESPACE ut
#endif

namespace UT_NAMESPACE {

	namespace log {

		//The basic log format is:
		//Suite:Test:file:line: function(): explanation

		template<typename Out> void getSuite(Notification const& n, Out o) {
			std::deque<char> const& suiteName = getSuiteNotification(n).getExpected();
			std::copy(suiteName.begin(), suiteName.end(), o);
		}

		template<typename Out> void getTest(Notification const& n, Out o) {
			std::deque<char> const& testName = getTestNotification(n)->getExpected();
			std::copy(testName.begin(), testName.end(), o);
		}

		template<typename Out> void getFile(Notification const& n, Out o) {
			const Notification::Where* where;
			n.where(&where);
			std::deque<char> const& file = where->getFile();
			std::copy(file.begin(), file.end(), o);
		}

		unsigned getLine(Notification const& n) {
			const Notification::Where* where;
			n.where(&where);
			return where->getLine();
		}

		template<typename Out> void getFunction(Notification const& n, Out o) {
			const Notification::Where* where;
			n.where(&where);
			std::deque<char> const& function = where->getFunction();
			std::copy(function.begin(), function.end(), o);
		}

		template<typename Out> void getExplanation(Notification const& n, Out o) {

		}

	} //namespace log

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_NOTIFICATION_LOG_HPP_ */
