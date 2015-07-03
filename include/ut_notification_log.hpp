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

		template<typename Out> void getSuite(Notification const& n, Out o) {
			std::deque<char> const& suiteName = getSuiteNotification(n)->getExpected();
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
			Notification::AssertionType at;
			n.assertionType(&at);
			std::deque<char> const& expected = n.getExpected();
			switch (at) {
			case Notification::Assert: {
				std::copy(expected.begin(), expected.end(), o);
				copyz(" is ", o);
				copyz(n.result() == Notification::Succeeded ? "expectedly true" : "unexpectedly false", o);
				break;
			}
			case Notification::AssertEquals: {
				break;
			}
			case Notification::AssertNotEquals: {
				break;
			}
			case Notification::AssertThrown: {
				break;
			}
			case Notification::AssertThrownNothing: {
				break;
			}
			} //switch
		} //getExplanation()

		//The basic log format is:
		//Suite:Test:file:line: function(): explanation
		template<typename S, typename Out> void getFull(Notification const& n, Out o) {
			getSuite(n, o);
			if (n.type() != Notification::SuiteStarted && n.type() != Notification::SuiteFinished) {
				copyz(":", o);
				getTest(n, o);
				if (n.type() != Notification::TestStarted && n.type() != Notification::TestFinished) {
					copyz(":", o);
					getFile(n, o);
					copyz(":", o);
					S::serialize(getLine(n), o);
					copyz(": ", o);
					getFunction(n, o);
					copyz("(): ", o);
					getExplanation(n, o);
				}
			}
		} //getFull()

	} //namespace log

} //namespace UT_NAMESPACE

#endif /* INCLUDE_UT_NOTIFICATION_LOG_HPP_ */
