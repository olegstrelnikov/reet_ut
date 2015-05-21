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
#define UT_ASSERT_NOT_THROWN(exceptionClass) NothingThrown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
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
#define UT_ASSERT_THROWN_NOT(exceptionClass) NothingThrown(#exceptionClass, typeid((exceptionClass)), __FILE__, __func__, __LINE__);\
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

	};

	template<typename T> class Context : public ContextBase {

	};

	class Runner;

	class Suite {
	public:
		void run();
		void setRunner(Runner* runner) {
			runner_ = runner;
		}
	protected:
		Suite() : runner_(0) {}
		void Assert(const char* expression, bool assertion, const char* file, const char* function, size_t line) {

		}
	private:
		Runner* runner_;
		const char* currentTest_;
	}; //class Suite

	class Runner {
	public:
		Runner() {};
		~Runner() {
			report_(std::cout);
		}
		template<typename SuiteT> void add() {
			SuiteT psuite = new SuiteT;
			psuite->setRunner(this);
			suites_.emplace_back(psuite);
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
