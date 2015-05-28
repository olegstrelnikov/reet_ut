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
	private:
		void notify_(std::unique_ptr<Notification>&& n) override {
			switch (n->type()) {
				case Notification::SuiteStarted: {
					os_ << "Suite " << n->getName() << " started\n";
					break;
				}
				case Notification::SuiteFinished: {
					os_ << "Suite " << n->getName() << " finished\n";
					os_ << "\n";
					break;
				}
				case Notification::TestStarted: {
					os_ << n->getName() << ": ";
					break;
				}
				case Notification::TestFinished: {
					os_ << " - finished";
					if (n->hasException()) {
						os_ << " (thrown expected";
						if (n->exceptionMessage()) {
							os_ << " \"" << n->exceptionMessage() << "\"";
						}
						os_ << ")";
					}
					os_ << "\n";
					break;
				}
				case Notification::TestAborted: {
					os_ << " - aborted";
					if (n->hasException()) {
						os_ << " (thrown unexpected";
						if (n->exceptionMessage()) {
							os_ << " \"" << n->exceptionMessage() << "\"";
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

		}
		void suitesSummary_() {

		}
		void report_() {
			listOfFailures_();
			os_ << "\n";
			testsSummary_();
			os_ << "\n";
			suitesSummary_();
		}
		std::ostream& os_;
	};
} //namespace

#endif /* UT_STREAMS_HPP_ */
