/*
 * ut_streams.hpp
 *
 *  Created on: May 27, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef UT_STREAMS_HPP_
#define UT_STREAMS_HPP_

#include <iostream>

#include "ut.hpp"

#ifndef UT_NAMESPACE
#define UT_NAMESPACE ut
#endif

namespace UT_NAMESPACE {

	class StreamsCollector : public Collector {
	public:
		StreamsCollector(std::ostream& os = std::cout) : os_(os) {}
		~StreamsCollector() { report_(); }
	private:
		void notify_(std::unique_ptr<Notification>&& n) override {
			switch (n->type()) {
				case Notification::SuiteStarted: {
					os_ << "Suite ";
					std::deque<char> const& name = n->getExpected();
					std::copy(std::begin(name), std::end(name), std::ostream_iterator<char>(os_));
					os_ << " started\n";
					break;
				}
				case Notification::SuiteFinished: {
					os_ << "Suite ";
					std::deque<char> const& name = n->getExpected();
					std::copy(std::begin(name), std::end(name), std::ostream_iterator<char>(os_));
					os_ << " finished\n";
					os_ << "\n";
					break;
				}
				case Notification::TestStarted: {
					std::deque<char> const& name = n->getExpected();
					std::copy(std::begin(name), std::end(name), std::ostream_iterator<char>(os_));
					std::deque<char> const* pClass;
					if (n->expectedException(&pClass)) {
						os_ << " (expect<";
						std::copy(std::begin(*pClass), std::end(*pClass), std::ostream_iterator<char>(os_));
						os_ << ">)";
					}
					os_ << ": ";
					break;
				}
				case Notification::TestFinished: {
					os_ << " - finished";
					std::deque<char> const* pClass;
					std::deque<char> const* pMessage;
					if (n->thrownException(&pClass, &pMessage)) {
						os_ << " (expected";
						os_ << "<";
						std::copy(std::begin(*pClass), std::end(*pClass), std::ostream_iterator<char>(os_));
						os_ << ">";
						if (!pMessage->empty()) {
							os_ << "(";
							std::copy(std::begin(*pMessage), std::end(*pMessage), std::ostream_iterator<char>(os_));
							os_ << ")";
						}
						os_ << ")";
					}
					os_ << "\n";
					break;
				}
				case Notification::TestAborted: {
					os_ << " - aborted";
					std::deque<char> const* pClass;
					std::deque<char> const* pMessage;
					if (n->thrownException(&pClass, &pMessage)) {
						os_ << " (unexpected";
						os_ << "<";
						std::copy(std::begin(*pClass), std::end(*pClass), std::ostream_iterator<char>(os_));
						os_ << ">";
						if (!pMessage->empty()) {
							os_ << "(";
							std::copy(std::begin(*pMessage), std::end(*pMessage), std::ostream_iterator<char>(os_));
							os_ << ")";
						}
						os_ << ")";
					}
					os_ << "\n";
					break;
				}
				case Notification::AssertionSucceeded: {
					os_ << '.';
					break;
				}
				case Notification::AssertionFailed: {
					os_ << 'F';
					break;
				}
			} //switch
			Collector::notify_(std::move(n));
		}
		void listOfFailures_() {

		}
		void testsSummary_() {
			os_ << "Tests started: " << getTestsStarted() << ", finished: " << getTestsFinished() << ", aborted: " << getTestsAborted();
		}
		void suitesSummary_() {
			os_ << "Suites started: " << getSuitesStarted() << ", finished: " << getSuitesFinished();
		}
		void report_() {
			listOfFailures_();
			os_ << "\n";
			testsSummary_();
			os_ << "\n";
			suitesSummary_();
			os_ << "\n";
		}
		std::ostream& os_;
	};
} //namespace

#endif /* UT_STREAMS_HPP_ */
