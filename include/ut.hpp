/*
 * ut.hpp
 *
 *  Created on: May 19, 2015
 *      Author: Oleg Strelnikov
 */

#ifndef UT_HPP_
#define UT_HPP_

#include <deque>
#include <algorithm> //copy, count_if
#include <memory> //unique_ptr
#include <iterator> //back_inserter
#include <functional> //bind

#define UT_ASSERT(assertion) runner_->Assert(#assertion, (assertion), __FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUAL(expected, actual) AssertEqual(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_ASERT_NOT_EQUAL(expected, actual) AssertNotEqual(#expected, #actual, (expected), (actual), __FILE__, __func__, __LINE__)

#define UT_ASSERT_THROWN(exceptionClass) NothingThrown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
	catch(exceptionClass& e) {\
		ThrownExpected(#exceptionClass, e, __FILE__, __func__, __LINE__);\
	} catch (...) {\
		try {\
			throw;\
		} catch (...) {\
			ThrownUnknown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
		}\
	}

#define UT_ASSERT_NOTHING_THROWN \
	catch (...) {\
		try {\
			throw;\
		} catch (...) {\
			ThrownUnknown(__FILE__, __func__, __LINE__);\
		}\
	}

#ifndef UT_NAMESPACE
#define UT_NAMESPACE ut
#endif

namespace UT_NAMESPACE {

	template<typename Out, typename CharT> void copyz(CharT const* p, Out o) {
		if (p) {
			for (; *p; ++p, ++o) {
				*o = *p;
			}
		}
	} //copyz()

	class Notification {
	public:
		enum Type {SuiteStarted, SuiteFinished, TestStarted, TestFinished, Assertion};
		enum TypeResult {Succeeded, Failed};
		enum AssertionType {Assert, AssertEquals, AssertNotEquals, AssertThrown, AssertThrownNothing};

		class Where {
		public:
			Where(const char* file, const char* function, unsigned line)
				: line_(line) {
				copyz(file, std::back_inserter(file_));
				copyz(function, std::back_inserter(function_));
			};
			std::deque<char> const& getFile() const {
				return file_;
			}
			std::deque<char> const& getFunction() const {
				return function_;
			}
			unsigned getLine() const {
				return line_;
			}
		private:
			std::deque<char> file_;
			std::deque<char> function_;
			unsigned line_;
		}; //class Notification::Where

		virtual ~Notification() {};
		Notification const& parent() const {
			return getParent_();
		}
		Type type() const {
			return getType_();
		}
		TypeResult result() const {
			return getResult_();
		}
		void assertionType(AssertionType* p) const {
			return getAssertionType_(p);
		}
		void where(Where const** ppWhere) const {
			return where_(ppWhere);
		}
		std::deque<char> const& getExpected() const {
			return getExpected_();
		}
		void getExpectedValue(std::deque<char> const** pp) const {
			return getExpectedValue_(pp);
		}
		void getActual(std::deque<char> const** pp) const {
			return getActual_(pp);
		}
		void getActualValue(std::deque<char> const** pp) const {
			return getActualValue_(pp);
		}
		bool thrownException(std::deque<char> const** ppExceptionClass, std::deque<char> const** ppMessage) const {
			return thrownException_(ppExceptionClass, ppMessage);
		}
		bool expectedException(std::deque<char> const** ppExceptionClass) const {
			return expectedException_(ppExceptionClass);
		}
	private:
		virtual Notification const& getParent_() const = 0;
		virtual Type getType_() const = 0;
		virtual TypeResult getResult_() const = 0;
		virtual void getAssertionType_(AssertionType*) const { }
		virtual void where_(Where const**) const { }
		virtual std::deque<char> const& getExpected_() const = 0;
		virtual void getExpectedValue_(std::deque<char> const**) const { }
		virtual void getActual_(std::deque<char> const**) const { }
		virtual void getActualValue_(std::deque<char> const**) const { }
		virtual bool thrownException_(std::deque<char> const** ppExceptionClass, std::deque<char> const**) const { return false; }
		virtual bool expectedException_(std::deque<char> const**) const { return false; }
	}; //class Notification

	class Collector {
	private:
		typedef std::deque<std::unique_ptr<Notification const>> ContainerT;
	public:
		virtual ~Collector() {}
		void notify(std::unique_ptr<Notification const>&& n) {
			notify_(std::move(n));
		}
		std::size_t getSuitesStarted() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::SuiteStarted; });
		}
		std::size_t getSuitesFinished() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::SuiteFinished; });
		}
		std::size_t getTestsStarted() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::TestStarted; });
		}
		std::size_t getTestsFinished() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::TestFinished && n->result() == Notification::Succeeded; });
		}
		std::size_t getTestsAborted() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::TestFinished && n->result() == Notification::Failed; });
		}
		std::size_t getAssertions() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::Assertion; });
		}
		std::size_t getAssertionsSucceeded() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::Assertion && n->result() == Notification::Succeeded; });
		}
		std::size_t getAssertionsFailed() const {
			return std::count_if(std::begin(n_), std::end(n_), [](typename ContainerT::value_type const& n) { return n->type() == Notification::Assertion && n->result() == Notification::Failed; });
		}
	protected:
		virtual void notify_(std::unique_ptr<Notification const>&& n) {
			n_.push_back(std::move(n));
		}
	private:
		ContainerT n_;
	}; //class Collector

	template<typename E> class What {
	public:
		typedef E Exception;
		template<typename Out> static void Message(Exception const& e, Out o) {
			copyz(e.what(), o);
		}
	}; //class What

	class RunnerBase {
	public:
		virtual ~RunnerBase() {}
		void Assert(const char* expression, bool assertion, const char* file, const char* function, unsigned line) {
			Assert_(expression, assertion, file, function, line);
		} //Runner<>::Assert_()
	private:
		virtual void Assert_(const char* expression, bool assertion, const char* file, const char* function, unsigned line) = 0;
	}; //class RunnerBase

	template<typename SuiteT> struct Test {
		typedef void (SuiteT::*type)();
	}; //struct Test

	template<typename SuiteT, typename... EHVocabulary> class Runner : public RunnerBase {
	public:
		template<typename... Strings> Runner(Collector& collector, const char* suiteName, Strings... strings)
			: currentTestNotification_(nullptr), suite_(), collector_(collector) {
			suite_.setContext(this);
			copyz(suiteName, std::back_inserter(suiteName_));
			TypeListManager::storeNamesTo(exceptions_, strings...);
		}

		void addTest(typename Test<SuiteT>::type test, const char* name) {
			tests_.emplace_back(test, name, &Runner::call_);
		}

		template<typename Exception> void addTestThrowing(typename Test<SuiteT>::type test, const char* name) {
			std::deque<char> exceptionClassName;
			ClassName<Exception, TypeListManager::hasName<Exception>()>::get(*this, std::back_inserter(exceptionClassName));
			tests_.emplace_back(test, name, &Runner::expect_<Exception>, exceptionClassName);
		}

		void run() {
			SuiteNotification const& n = *new SuiteNotification(suiteName_, Notification::SuiteStarted);
			collector_.notify(std::move(std::unique_ptr<SuiteNotification const>(&n)));
			for (TestRun& test : tests_) {
				currentTestNotification_ = new TestNotification(test, n);
				collector_.notify(std::move(std::unique_ptr<TestNotification const>(currentTestNotification_)));
				(this->*(test.caller_))(test);
				collector_.notify(std::move(std::unique_ptr<TestNotification const>(new TestNotification(test, n))));
			}
			collector_.notify(std::move(std::unique_ptr<SuiteNotification const>(new SuiteNotification(suiteName_, Notification::SuiteFinished, n))));
		} //Runner<>::run()

	private:
		class TypeListManager {
		public:
			template<typename... Strings> static void storeNamesTo(std::deque<char> names[], Strings... strings) {
				name_<EHVocabulary...>::store(names, strings...);
			}
			template<typename T, typename Out> static void getNameOf(std::deque<char> const names[], Out o) {
				std::deque<char> const& s = names[indexOf_<T>()];
				std::copy(s.begin(), s.end(), o);
			}
			template<typename T> static constexpr bool hasName() {
				return index_<T, EHVocabulary...>::has();
			}

		private:
			template<typename ...> struct index_ {
				static constexpr bool has() {
					return false;
				}
			};

			template<typename T1, typename T2, typename... TT> struct index_<T1, T2, TT...> {
				static constexpr std::size_t get() {
					return index_<T1, TT...>::get();
				}
				static constexpr bool has() {
					return index_<T1, TT...>::has();
				}
			};

			template<typename EH, typename... TT> struct index_<typename EH::Exception, EH, TT...> {
				static constexpr std::size_t get() {
					return 1 + sizeof... (TT);
				}
				static constexpr bool has() {
					return true;
				}
			};

			template<typename T> static constexpr std::size_t indexOf_() {
				return index_<T, EHVocabulary...>::get();
			}

			template<typename... TT> class name_ {
			public:
				static void store(std::deque<char> []) {} //implementation for Runner without vocabulary
			};

			template<typename EH> class name_<EH> {
			public:
				static void store(std::deque<char> names[], const char* s) {
					copyz(s, std::back_inserter(names[indexOf_<typename EH::Exception>()]));
				}
			};

			template<typename EH, typename... TT> class name_<EH, TT...> {
			public:
				template<typename... Strings> static void store(std::deque<char> names[], const char* s, Strings... strings) {
					name_<EH>::store(names, s);
					name_<TT...>::store(names, strings...);
				}
			};
		}; //class Runner<>::TypeListManager

		enum ExceptionHandlerResult {NothingThrown, ThrownKnown, ThrownUnknown, ThrownKnownExpected, ThrownUnknownExpected};

		template<typename... EHs> class Catch {
		public:
			template<typename Out, typename ClassOut> static ExceptionHandlerResult whatWasThrown(Out, ClassOut, Runner const&) {
				try {
					throw;
				} catch (...) {
					return ThrownUnknown;
				}
			}
		};

		template<typename EH, typename... EHs> class Catch<EH, EHs...> {
		public:
			template<typename Out, typename ClassOut> static ExceptionHandlerResult whatWasThrown(Out out, ClassOut classOut, Runner const& runner) {
				try {
					throw;
				} catch (typename EH::Exception& e) {
					runner.getNameOf<typename EH::Exception>(classOut);
					EH::Message(e, out);
					return ThrownKnown;
				} catch (...) {
					return Catch<EHs...>::whatWasThrown(out, classOut, runner);
				}
			}
		};

		template<typename FN, typename Out, typename ClassOut> ExceptionHandlerResult whatThrows(FN fn, Out out, ClassOut classOut) {
			try {
				fn();
				return NothingThrown;
			} catch (...) {
				return Catch<EHVocabulary...>::whatWasThrown(out, classOut, *this);
			}
		}

		template<typename E, typename FN, typename Out, typename ClassOut> ExceptionHandlerResult whetherThrows(FN fn, Out out, ClassOut classOut) {
			try {
				fn();
				return NothingThrown;
			} catch (...) {
				ExceptionHandlerResult r = Catch<EHVocabulary...>::whatWasThrown(out, classOut, *this);
				try {
					throw;
				} catch (E&) {
					return ThrownKnown == r ? ThrownKnownExpected : ThrownUnknownExpected;
				} catch (...) {
					return r;
				}
			}
		} //Runner<>::whetherThrows()

		template<typename E, typename Out> void getNameOf(Out o) const {
			return TypeListManager::getNameOf<E>(exceptions_, o);
		}

		template<typename E, bool known> class ClassName {
		public:
			template<typename Out> static void get(Runner const& runner, Out o) {
				runner.getNameOf<E>(o);
			}
		};

		template<typename E> class ClassName<E, false> {
		public:
			template<typename Out> static void get(Runner&, Out) {}
		};

		class TestRun {
		public:
			typedef void (Runner::*caller)(TestRun&);
			TestRun(typename Test<SuiteT>::type test, const char* name, caller call, bool expecting = false)
				: state_(NotStarted), test_(test), caller_(call), expecting_(expecting) {
				copyz(name, std::back_inserter(name_));
			};
			TestRun(typename Test<SuiteT>::type test, const char* name, caller call, std::deque<char> const& expected)
				: TestRun(test, name, call, true) {
				std::copy(std::begin(expected), std::end(expected), back_inserter(expected_));
			};
			enum {NotStarted, Running, NothingThrownAsExpected, CaughtExpected, CaughtUnexpected, NotThrownButExpected} state_;
			typename Test<SuiteT>::type test_;
			std::deque<char> name_;
			caller caller_;
			std::deque<char> thrownMessage_;
			std::deque<char> thrownClass_;
			bool const expecting_;
			std::deque<char> expected_;
			bool isFinished() const {
				return NothingThrownAsExpected == state_ || CaughtExpected == state_;
			}
			bool thrown() const {
				return CaughtExpected == state_ || CaughtUnexpected == state_;
			}
			Notification::Type notification() const {
				return NotStarted == state_ ? Notification::TestStarted : Notification::TestFinished;
			}
			Notification::TypeResult result() const {
				return isFinished() ? Notification::Succeeded : Notification::Failed;
			}
		}; //class Runner<>::TestRun

		class SuiteNotification : public Notification {
		public:
			SuiteNotification(std::deque<char> const& name, Notification::Type type, Notification const& parent)
				: suiteName_(name), type_(type), parent_(parent) {}
			SuiteNotification(std::deque<char> const& name, Notification::Type type)
				: SuiteNotification(name, type, *this) {}
		private:
			std::deque<char> suiteName_;
			Notification::Type type_;
			Notification const& parent_;
			virtual Notification const& getParent_() const override { return parent_; }
			Type getType_() const override { return type_; }
			virtual TypeResult getResult_() const override { return Notification::Succeeded; }
			virtual std::deque<char> const& getExpected_() const override { return suiteName_; }
		}; //class Runner<>::SuiteNotification

		class TestNotification : public Notification {
		public:
			TestNotification(TestRun const& r, Notification const& parent)
				: run_(r), type_(r.notification()), result_(r.result()), thrown_(r.thrown()), parent_(parent)  {}
		private:
			TestRun const& run_;
			Type const type_;
			TypeResult const result_;
			bool const thrown_;
			Notification const& parent_;
			virtual Notification const& getParent_() const override { return parent_; }
			Type getType_() const override { return type_; }
			virtual TypeResult getResult_() const override { return result_; }
			bool thrownException_(std::deque<char> const** ppClass, std::deque<char> const** ppMessage) const override {
				if (thrown_) {
					*ppMessage = &run_.thrownMessage_;
					*ppClass = &run_.thrownClass_;
				}
				return thrown_;
			}
			bool expectedException_(std::deque<char> const** ppExceptionClass) const override {
				if (run_.expecting_) {
					*ppExceptionClass = &run_.expected_;
				}
				return run_.expecting_;
			}
			std::deque<char> const& getExpected_() const override { return run_.name_; }
		}; //class Runner<>::TestNotification

		class Assertion : public Notification {
		public:
			Assertion(Notification const& parent, Notification::TypeResult result, const char* file, const char* function, unsigned line, const char* expected)
				: parent_(parent), result_(result), w_(file, function, line) {
				copyz(expected, std::back_inserter(expected_));
			}
		private:
			virtual Notification const& getParent_() const { return parent_; }
			virtual Type getType_() const { return Notification::Assertion; }
			virtual TypeResult getResult_() const { return result_; }
			virtual void getAssertionType_(AssertionType* t) const { *t = Notification::Assert; }
			virtual void where_(Where const** w) const { *w = &w_; }
			virtual std::deque<char> const& getExpected_() const { return expected_; }

			Notification const& parent_;
			Notification::TypeResult result_;
			Notification::Where w_;
			std::deque<char> expected_;
		}; //class Runner<>::Assertion

		void call_(TestRun& test) {
			test.state_ = TestRun::Running;
			std::deque<char> className, classValue;
			ExceptionHandlerResult r = whatThrows(std::bind(test.test_, suite_), std::back_inserter(classValue), std::back_inserter(className));
			if (NothingThrown == r) {
				test.state_ = TestRun::NothingThrownAsExpected;
			} else {
				test.state_ = TestRun::CaughtUnexpected;
				if (ThrownKnown == r) {
					std::copy(className.begin(), className.end(), std::back_inserter(test.thrownClass_));
					std::copy(classValue.begin(), classValue.end(), std::back_inserter(test.thrownMessage_));
				}
			}
		} // Runner<>::call_()
		template<typename Exception> void expect_(TestRun& test) {
			test.state_ = TestRun::Running;
			std::deque<char> className, classValue;
			ExceptionHandlerResult r = whetherThrows<Exception>(std::bind(test.test_, suite_), std::back_inserter(classValue), std::back_inserter(className));
			if (NothingThrown == r) {
				test.state_ = TestRun::NotThrownButExpected;
			} else {
				if (ThrownKnownExpected == r || ThrownUnknownExpected == r) {
					test.state_ = TestRun::CaughtExpected;
				} else {
					test.state_ = TestRun::CaughtUnexpected;
				}
				if (ThrownKnownExpected == r || ThrownKnown == r) {
					std::copy(className.begin(), className.end(), std::back_inserter(test.thrownClass_));
					std::copy(classValue.begin(), classValue.end(), std::back_inserter(test.thrownMessage_));
				}
			}
		} // Runner<>::expect_()

		void Assert_(const char* expression, bool assertion, const char* file, const char* function, unsigned line) override {
			collector_.notify(std::move(std::unique_ptr<Notification const>(new Assertion(*currentTestNotification_, assertion ? Notification::Succeeded : Notification::Failed, file, function, line, expression))));
		} //Runner<>::Assert_()

		TestNotification const* currentTestNotification_;
		std::deque<TestRun> tests_;
		SuiteT suite_;
		Collector& collector_;
		std::deque<char> suiteName_;
		std::deque<char> exceptions_[1 + sizeof... (EHVocabulary)];
	}; //class Runner<>

	class Suite {
	public:
		void setContext(RunnerBase* context) {
			runner_ = context;
		}
	protected:
		Suite() : runner_(nullptr) {}
	protected:
		RunnerBase* runner_;
	}; //class Suite

} //namespace ut

#endif /* UT_HPP_ */
