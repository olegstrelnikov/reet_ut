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
#include <ut_equal.hpp>

class InitialSuite : public ut::Suite {
public:

	void testAssertions() {
		UT_ASSERT(true);
		UT_ASSERT(false);
		int const N = 5;
		UT_ASSERT(N == 5);
		UT_ASSERT(N != 5);
	}

	void testEquality() {
#if 0
		static const int N = 10;
		int* n = new int(10);
	    UT_ASSERT( *n == N );
	    UT_ASSERT( !(*n == N + 1) );
	    UT_ASSERT_EQUAL(*n, 10);
	    delete n;
#endif
	}

	inline static void run(ut::Collector& collector) {
		ut::Runner<InitialSuite> runner(collector, "InitialSuite");
		runner.addTest(&InitialSuite::testAssertions, "testAssertions");
		runner.addTest(&InitialSuite::testEquality, "testEquality");
		runner.run();
	}

}; //class InitialSuite

class ExceptionsSuite : public ut::Suite {
public:
	void testNothing() {}

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
		runner.run();
	}

}; //class ExceptionsSuite

class Example {
public:
	static void run() {
		ut::StreamsCollector collector;
		InitialSuite::run(collector);
		ExceptionsSuite::run(collector);
	}
};

class EqualExample {
public:
	static void run() {
#define EQ(...) std::cout << "equal(" #__VA_ARGS__ ") = " << ut::equal(__VA_ARGS__) << "\n"
#define DECLARE(...) std::cout << #__VA_ARGS__ "\n"; __VA_ARGS__
	    EQ(1, 2);
	    EQ(2, 2);
	    //EQ(2, "hello"); //compiler error: 2.begin() not found
	    //EQ("hello", 3); //compiler error: 3.begin() not found
	    EQ(2, 2.2);
	    EQ('d', 'e');
	    EQ("hello", "hello");
	    DECLARE(int a[] = {1, 2, 3};)
	    DECLARE(int b[] = {1, 2, 3};)
	    EQ(a, b);
	    DECLARE(int c[] = {1, 2};)
	    //EQ(a, c); //linker error
	    EQ({1, 2, 3}, c);
	    DECLARE(const char* x1 = "world";)
	    EQ(x1, "world");
	    DECLARE(const char x2[] = "world";)
	    EQ(x2, "world");
	    EQ(x2, x1);
	    EQ({1, 2, 3}, {1, 2, 3});
	    EQ({1, 2, 3}, {1, 2});
	    EQ({1, 2, 3}, {1, 2, 4});
	    EQ(a, {1, 2, 3});
	    EQ({1, 2, 3}, a);
	    EQ(a, {1, 2, 3, 4});


	}
#undef DECLARE
#undef EQ
}; //class EqualExample

#endif /* EXAMPLE_UT_EXAMPLE_HPP_ */
