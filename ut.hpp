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
#include <memory>

#define UT_ASSERT(assertion) Assert(#assertion, (assertion), __FILE__, __func__, __LINE__)

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

#define UT_SUITE_BEGIN void addTests() {
#define UT_TEST(cls, test) addTest<cls>((test), (const char*)#test)
#define UT_SUITE_END }

namespace ut {

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

	class ContextBase {
	public:
		virtual ~ContextBase() {}
		void run() {
			run_();
		}
	private:
		virtual void run_() = 0;
	};

	template<typename SuiteT> struct Test {
		typedef void (SuiteT::*type)();
	};

	template<typename SuiteT> class Context : public ContextBase {
	public:
		Context(SuiteT* suite) : suite_(suite) {}
		void addTest(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name);
		}
	private:
		class TestTraits {
			TestTraits(typename Test<SuiteT>::type test, const char* name) : test_(test), name_(name) {};
			typename Test<SuiteT>::type test_;
			const char* name_;
		};
		void run_() override {
			for (auto test : tests_) {
				suite_->setCurrentTest(test.name_);
				suite_->*(test.test_)();
			}
		}
		std::deque<TestTraits> tests_;
		SuiteT* suite_;
	};

	class Runner;

	class Suite {
	public:
		void run() {
			context_->run();
		}
		void setRunner(Runner* runner) {
			runner_ = runner;
		}
		void setContext(ContextBase* context) {
			context_ = context;
		}
	protected:
		Suite() : runner_(0), currentTest_(0), context_(0) {}
		template<typename SuiteT> void addTest(typename Test<SuiteT>::type test, const char* name) {
			dynamic_cast<Context<SuiteT>*>(context_)->addTest(test);
		}
		void Assert(const char* expression, bool assertion, const char* file, const char* function, size_t line) {

		}
	private:
		Runner* runner_;
		const char* currentTest_;
		ContextBase* context_;
	}; //class Suite

	class Runner {
	public:
		Runner() {};
		~Runner() {
			report_(std::cout);
		}
		template<typename SuiteT> void add() {
			SuiteT suite = new SuiteT;
			suite->setRunner(this);
			suite->setContext(new Context<SuiteT>(suite));
			suites_.emplace_back(suite);
		}
		void run() {
			for (auto& psuite : suites_) {
				psuite->run();
			}
		}
	private:
		std::deque<std::unique_ptr<Suite>> suites_;
		std::deque<ReportLine> results_;
		std::ostream& report_(std::ostream& os) {
			return os;
		}
	}; //class Runner

} //namespace ut

#endif /* UT_HPP_ */
