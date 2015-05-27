/*
 * ut.hpp
 *
 *  Created on: May 19, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef UT_HPP_
#define UT_HPP_

#include <deque>
#include <algorithm>
#include <iostream>
#include <exception>

#define UT_ASSERT(assertion) runner_->Assert(#assertion, (assertion), __FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUALS(expected, actual) AssertEquals(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_ASERT_NOT_EQUALS(expected, actual) AssertNotEquals(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_ASSERT_THROWN(exceptionClass) NothingThrown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
	catch(exceptionClass& e) {\
		ThrownExpected(#exceptionClass, e, __FILE__, __func__, __LINE__);\
	} catch (...) {\
		try {\
			throw;\
		} catch {std::exception& e} {\
			ThrownUnexpectedStdException(#exceptionClass, typeid((exceptionClass)), e, __FILE__, __func__, __LINE__);\
		} catch (...) {\
			ThrownUnknown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
		}\
	}
#define UT_ASSERT_NOTHING_THROWN NothingThrown(__FILE__, __func__, __LINE__);\
	catch (...) {\
		try {\
			throw;\
		} catch {std::exception& e} {\
			ThrownUnexpectedStdException(e, __FILE__, __func__, __LINE__);\
		} catch (...) {\
			ThrownUnknown(__FILE__, __func__, __LINE__);\
		}\
	}

#ifndef UT_NAMESPACE
#define UT_NAMESPACE ut
#endif

namespace UT_NAMESPACE {

	class Suite;

	class Where {
	public:
		Where(const char* file, const char* function, size_t line, const char* initialFunction, Suite const* psuite)
			: file_(file), function_(function), line_(line), testName_(initialFunction), psuite_(psuite) {};
		std::ostream& str(std::ostream& os) const {
			return os << file_ << ":" << function_ << "():" << line_ << ": ";
		}
	private:
		const char* file_;
		const char* function_;
		size_t line_;
		const char* testName_;
		Suite const* psuite_;
	};

	enum TestReult {UnexpectedException, };
	class ReportLine {
	public:
		enum What {Ok, Fail, AnotherExceptionExpected, NotThrown};
	private:
		What what_;
		Where where_;
	};

	class RunnerBase {
	public:
		virtual ~RunnerBase() {}
		void Assert(const char* expression, bool assertion, const char* file, const char* function, size_t line) {
			Assert_(expression, assertion, file, function, line);
		}
	private:
		virtual void Assert_(const char* expression, bool assertion, const char* file, const char* function, size_t line) = 0;
	};

	template<typename SuiteT> struct Test {
		typedef void (SuiteT::*type)();
	};

	template<typename SuiteT> class Runner : public RunnerBase {
	public:
		Runner(std::ostream& os = std::cout) : currentTest_(0), suite_(), os_(os) {suite_.setContext(this);}
		void addTest(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name, &Runner::call_);
		}
		template<typename Exception>void addTestThrowing(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name, &Runner::expect_<Exception>);
		}
		void run() {
			for (TestRun& test : tests_) {
				os_ << (currentTest_ = test.name_) << ": ";
				(this->*(test.caller_))(test);
				os_ << " - " << (test.isFinished() ? "finished" : "aborted");
				if (test.thrown()) {
					os_ << " (thrown " <<  (test.isFinished() ? "expected" : "unexpected");
					if (test.thrownMessage_) {
						os_ << " \"" << test.thrownMessage_ << "\"";
					}
					os_ << ")";
				}
				os_ << "\n";
			}
		}
		~Runner() { report_(os_); }
	private:
		class TestRun {
		public:
			typedef void (Runner::*caller)(TestRun&);
			TestRun(typename Test<SuiteT>::type test, const char* name, caller call)
				: state_(NotStarted), test_(test), name_(name), caller_(call), thrownMessage_(0) {};
			enum {NotStarted, Running, NothingThrownAsExpected, CaughtExpected, CaughtUnexpected, NotThrownButExpected} state_;
			typename Test<SuiteT>::type test_;
			const char* name_;
			caller caller_;
			const char* thrownMessage_;
			bool isFinished() const {
				return NothingThrownAsExpected == state_ || CaughtExpected == state_;
			}
			bool thrown() const {
				return CaughtExpected == state_ || CaughtUnexpected == state_;
			}

		}; //class TestRun
		void call_(TestRun& test) {
			try {
				test.state_ = TestRun::Running;
				(suite_.*(test.test_))();
				test.state_ = TestRun::NothingThrownAsExpected;
			} catch (...) {
				test.state_ = TestRun::CaughtUnexpected;
				try {
					throw;
				} catch (std::exception& e) {
					test.thrownMessage_ = e.what();
				} catch (...) {
				}
			}
		} //call_()
		template<typename Exception> void expect_(TestRun& test) {
			try {
				test.state_ = TestRun::Running;
				(suite_.*(test.test_))();
				test.state_ = TestRun::NotThrownButExpected;
			} catch (Exception&) {
				test.state_ = TestRun::CaughtExpected;
				try {
					throw;
				} catch (std::exception& e) {
					test.thrownMessage_ = e.what();
				} catch (...) {
				}
			} catch (...) {
				test.state_ = TestRun::CaughtUnexpected;
				try {
					throw;
				} catch (std::exception& e) {
					test.thrownMessage_ = e.what();
				} catch (...) {
				}
			}
		} //expect_()
		const char* currentTest_;
		std::deque<TestRun> tests_;
		SuiteT suite_;
		std::ostream& os_;
		void report_(std::ostream& os) {

		}
		virtual void Assert_(const char* expression, bool assertion, const char* file, const char* function, size_t line) {

		}
	}; //class Runner

	class Suite {
	public:
		void setContext(RunnerBase* context) {
			runner_ = context;
		}
	protected:
		Suite() : runner_(0) {}
	private:
		RunnerBase* runner_;
	}; //class Suite

} //namespace ut

#endif /* UT_HPP_ */
