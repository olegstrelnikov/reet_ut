/*
 * ut_example.hpp
 *
 *  Created on: Jun 18, 2015
 *      Author: ois
 */

#ifndef EXAMPLE_UT_EXAMPLE_HPP_
#define EXAMPLE_UT_EXAMPLE_HPP_

#include <ut.hpp>
#include <ut_streams.hpp>
#include <ut_stream_handler.hpp>

class ExceptionsSuite : public ut::Suite {
public:
	void testNothing() {}

	void testEquality()
	{
#if 0
		static const int N = 10;
		int* n = new int(10);
	    UT_ASSERT( *n == N );
	    UT_ASSERT( !(*n == N + 1) );
	    UT_ASSERT_EQUAL(*n, 10);
	    delete n;
#endif
	}

	void testException() {
		class T1 { public: virtual ~T1() {} };
		class T2 : public T1 { };
		T1 const& r = T1();
		dynamic_cast<T2 const&>(r);
	}

	void testIntException() {
		throw 5;
	}

	inline static void run(ut::Collector& collector) {
		ut::Runner<ExceptionsSuite, ut::What<std::bad_cast>, ut::What<std::bad_alloc>, ut::What<std::exception>, ut::Stream<int>>
			runner(collector, "ExceptionsSuite", "std::bad_cast", "std::bad_alloc", "std::exception", "int");
		runner.addTest(&ExceptionsSuite::testNothing, "testNothing");
		runner.addTestThrowing<int>(&ExceptionsSuite::testNothing, "testNothing 2");
		runner.addTestThrowing<std::exception>(&ExceptionsSuite::testException, "testException");
		runner.addTestThrowing<std::bad_alloc>(&ExceptionsSuite::testException, "testException 2");
		runner.addTestThrowing<int>(&ExceptionsSuite::testException, "testException 3");
		runner.addTestThrowing<std::bad_cast>(&ExceptionsSuite::testException, "testException 4");
		runner.addTest(&ExceptionsSuite::testException, "testException 5");
		runner.addTestThrowing<std::bad_alloc>(&ExceptionsSuite::testIntException, "testIntException");
		runner.addTestThrowing<int>(&ExceptionsSuite::testIntException, "testIntException 2");
		runner.addTest(&ExceptionsSuite::testEquality, "testEquality");
		runner.run();
	}

}; //class InitialSuite

class Example {
public:
	static void run() {
		ut::StreamsCollector collector;
		ExceptionsSuite::run(collector);
	}
};
#endif /* EXAMPLE_UT_EXAMPLE_HPP_ */
