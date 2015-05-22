/*
 * ut.hpp
 *
 *  Created on: May 19, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef UT_HPP_
#define UT_HPP_

#include <iostream>
#include <deque>

#define UT_ASSERT(assertion) runner_->Assert(#assertion, (assertion), __FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUALS(expected, actual) AssertEquals(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_ASERT_NOT_EQUALS(expected, actual) AssertNotEquals(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_TRY try {
#define UT_ASSERT_THROWN(exceptionClass) NothingThrown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
	} catch(exceptionClass& e) {\
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
	} catch (...) {\
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
		Runner(std::ostream& os = std::cout) : os_(os), currentTest_(0) {suite_.setContext(this);}
		void addTest(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name);
		}
		void run() {
			for (auto test : tests_) {
				currentTest_ = test.name_;
				(suite_->*(test.test_))();
			}
		}
		~Runner() { report_(os_); }
	private:
		class TestRun {
		public:
			TestRun(typename Test<SuiteT>::type test, const char* name) : test_(test), name_(name) {};
			typename Test<SuiteT>::type test_;
			const char* name_;
		};
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
#if 0
		template<typename SuiteT> void addTest(typename Test<SuiteT>::type test, const char* name) {
			dynamic_cast<Runner<SuiteT>*>(runner_)->addTest(test, name);
		}
#endif
	private:
		RunnerBase* runner_;
	}; //class Suite

} //namespace ut

#endif /* UT_HPP_ */
