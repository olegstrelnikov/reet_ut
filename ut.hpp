/*
 * ut.hpp
 *
 *  Created on: 19 ��� 2015 �.
 *      Author: ois
 */

#ifndef UT_HPP_
#define UT_HPP_

#include <iostream>

#define UT_ASSERT(assertion) Where(__FILE__, __func__, __LINE__)

#define UT_ASSERT_EQUALS(expected, actual)

#define UT_ASERT_NOT_EQUALS(expected, actual)


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
		static void run();
	};
}


#endif /* UT_HPP_ */
