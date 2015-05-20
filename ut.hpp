/*
 * ut.hpp
 *
 *  Created on: 19 мая 2015 г.
 *      Author: ois
 */

#ifndef UT_HPP_
#define UT_HPP_

#include <iostream>
#include <deque>

#define UT_ASSERT(assertion) Where(__FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUALS(expected, actual) Where(__FILE__, __func__, __LINE__)

#define UT_ASERT_NOT_EQUALS(expected, actual) Where(__FILE__, __func__, __LINE__)


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
	class Runner {
	public:
		void run();
	private:
		std::deque<ReportLine> results_;
		std::ostream& report_(std::ostream& os) {
			return os;
		}
	};
}


#endif /* UT_HPP_ */
