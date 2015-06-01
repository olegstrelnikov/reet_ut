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
#include <memory>

#define UT_ASSERT(assertion) runner_->Assert(#assertion, (assertion), __FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUAL(expected, actual) AssertEqual(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_ASERT_NOT_EQUAL(expected, actual) AssertNotEqual(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

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

	class Where {
	public:
		Where(const char* file, const char* function, unsigned line, const char* initialFunction)
			: file_(file), function_(function), line_(line), testName_(initialFunction) {};
	private:
		const char* file_;
		const char* function_;
		unsigned line_;
		const char* testName_;
	};

	class Notification {
	public:
		enum Type {SuiteStarted, SuiteFinished, TestStarted, TestFinished, TestAborted, AssertionSucceeded, AssertionFailed};
		virtual ~Notification() {};
		Type type() const {
			return getType_();
		}
		bool hasException() const {
			return hasException_();
		}
		const char* exceptionMessage() const {
			return getExceptionMessage_();
		}
		const char* getName() const {
			return getName_();
		}
	private:
		virtual Type getType_() const = 0;
		virtual bool hasException_() const { return false; }
		virtual const char* getExceptionMessage_() const { return nullptr; }
		virtual const char* getName_() const { return nullptr; }
	}; //class Notification

	class Collector {
	public:
		virtual ~Collector() {}
		void notify(std::unique_ptr<Notification>&& n) {
			notify_(std::move(n));
		}
	protected:
		virtual void notify_(std::unique_ptr<Notification>&& n) {
			n_.push_back(std::move(n));
		}
	private:
		std::deque<std::unique_ptr<Notification>> n_;
	};

	class ReportLine {
	public:
		enum What {Ok, Fail, AnotherExceptionExpected, NotThrown};
	private:
		What what_;
		Where where_;
	};

	template<typename Serializer>
	class ExceptionHandler {
	public:
		typedef typename Serializer::Exception Exception;
		template<typename Out> inline static void Serialize(Exception const& e, Out out) { Serializer::what(e, out); }
	};

	template<typename Out, typename CharT> void copyz(CharT const* p, Out o) {
		if (p) {
			for (; *p; ++p, ++o) {
				*o = *p;
			}
		}
	}

	template<typename E> class What {
	public:
		typedef E Exception;
		template<typename Out> static void what(Exception const& e, Out o) {
			copyz(e.what(), o);
		}
		template<typename ClassOut> static void getClass(ClassOut o) {
#define UT_CLASS_NAME(className) #className
			copyz(UT_CLASS_NAME(E), o);
#undef UT_CLASS_NAME
		}
	};

	class RunnerBase {
	public:
		virtual ~RunnerBase() {}
		void Assert(const char* expression, bool assertion, const char* file, const char* function, unsigned line) {
			Assert_(expression, assertion, file, function, line);
		}
	private:
		virtual void Assert_(const char* expression, bool assertion, const char* file, const char* function, unsigned line) = 0;
	};

	template<typename SuiteT> struct Test {
		typedef void (SuiteT::*type)();
	};

	template<typename SuiteT> class Runner : public RunnerBase {
	public:
		Runner(Collector& collector, const char* suiteName) : currentTest_(nullptr), suite_(), collector_(collector), suiteName_(suiteName) {suite_.setContext(this);}
		void addTest(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name, &Runner::call_);
		}
		template<typename Exception>void addTestThrowing(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name, &Runner::expect_<Exception>);
		}
		void run() {
			collector_.notify(std::move(std::unique_ptr<SuiteNotification>(new SuiteNotification(suiteName_, true))));
			for (TestRun& test : tests_) {
				collector_.notify(std::move(std::unique_ptr<TestNotification>(new TestNotification(currentTest_ = test.name_, test))));
				(this->*(test.caller_))(test);
				collector_.notify(std::move(std::unique_ptr<TestNotification>(new TestNotification(currentTest_, test))));
			}
			collector_.notify(std::move(std::unique_ptr<SuiteNotification>(new SuiteNotification(suiteName_, false))));
		}
	private:

		enum ExceptionHandlerResult {NothingThrown, ThrownKnown, ThrownUnknown};

		template<typename... EHs> class Catch;

		template<typename EH, typename... EHs> class Catch<EH, EHs...> {
		public:
			template<typename Out, typename ClassOut> static ExceptionHandlerResult whatWasThrown(Out out, ClassOut classOut) {
				try {
					throw;
				} catch (typename EH::Exception& e) {
					EH::Serialize(e, out);
					return ThrownKnown;
				} catch (...) {
					return Catch<EHs...>::whatWasThrown(out);
				}
			}
		};

		template<> class Catch<> {
		public:
			template<typename Out, typename ClassOut> static ExceptionHandlerResult whatWasThrown(Out, ClassOut) {
				try {
					throw;
				} catch (...) {
					return ThrownUnknown;
				}
			}
		};

		template<typename FN, typename Out, typename ClassOut, typename... EHs> ExceptionHandlerResult whatThrowns(FN fn, Out out, ClassOut classOut) {
			try {
				fn();
				return NothingThrown;
			} catch (...) {
				return Catch<EHs...>::whatWasThrown(out, classOut);
			}
		}

		class TestRun {
		public:
			typedef void (Runner::*caller)(TestRun&);
			TestRun(typename Test<SuiteT>::type test, const char* name, caller call)
				: state_(NotStarted), test_(test), name_(name), caller_(call), thrownMessage_(nullptr) {};
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
			Notification::Type notification() const {
				return NotStarted == state_ ? Notification::TestStarted : isFinished() ? Notification::TestFinished : Notification::TestAborted;
			}
		}; //class TestRun

		class SuiteNotification : public Notification {
		public:
			SuiteNotification(const char* name, bool start) : suiteName_(name), start_(start) {}
		private:
			const char* suiteName_;
			bool start_;
			Type getType_() const override { return start_ ? SuiteStarted : SuiteFinished; }
			const char* getName_() const override { return suiteName_; }
		}; //class SuiteNotification

		class TestNotification : public Notification {
		public:
			TestNotification(const char* name, TestRun const& r) : testName_(name), type_(r.notification()), thrown_(r.thrown()), exceptionMessage_(r.thrownMessage_) {}
		private:
			const char* testName_;
			Type type_;
			bool thrown_;
			const char* exceptionMessage_;
			Type getType_() const override { return type_; }
			bool hasException_() const override { return thrown_; }
			const char* getExceptionMessage_() const override { return exceptionMessage_; }
			const char* getName_() const override { return testName_; }
		}; //class TestNotification

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
		Collector& collector_;
		const char* suiteName_;

		unsigned getFinished_() const {
			return 0;
		}
		unsigned getAborted_() const {
			return 0;
		}
		std::ostream& testTotals_(std::ostream& os) const {
			return os << "Test runs: " << tests_.size() << ", finished: " << getFinished_() << ", aborted: " << getAborted_();
		}
		std::ostream& report_(std::ostream& os) const {
			return testTotals_(os) << "\n";
		}
		virtual void Assert_(const char* expression, bool assertion, const char* file, const char* function, unsigned line) {

		}
	}; //class Runner

	class Suite {
	public:
		void setContext(RunnerBase* context) {
			runner_ = context;
		}
	protected:
		Suite() : runner_(nullptr) {}
	private:
		RunnerBase* runner_;
	}; //class Suite

} //namespace ut

#endif /* UT_HPP_ */
