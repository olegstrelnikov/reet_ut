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

#define UT_ASSERT(assertion) Where(__FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUALS(expected, actual) Where(__FILE__, __func__, __LINE__)

#define UT_ASERT_NOT_EQUALS(expected, actual) Where(__FILE__, __func__, __LINE__)

#define UT_TRY try {
#define UT_ASSERT_CAUGHT(exceptionClass) } catch(exceptionClass&) {\
} catch {std::exception& e} {\


namespace ut {

	class Where {
	public:
		Where(const char* file, const char* function, size_t line) : file_(file), function_(function), line_(line) {};
		std::ostream& str(std::ostream& os) const {
			return os << file_ << ":" << function_ << "():" << line_ << ": ";
		}
	private:
		const char* file_;
		const char* function_;
		size_t line_;
	};

	class ReportLine {
	public:
		enum What {Ok, Fail, UnexpectedException, AnotherExceptionExpected};
	private:

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
	private:
		Runner* runner_;
	};

	class Runner {
	public:
		Runner() {};
		~Runner() {
			report_(std::cout);
		}
		void add(Suite& suite) {
			suite.setRunner(this);
			suites_.emplace_back(new Suite(suite));
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
	};

} //namespace ut


#endif /* UT_HPP_ */
