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
#define EQ(expected, ...) {\
	bool result = ut::equal(__VA_ARGS__);\
	std::cout << "equal(" #__VA_ARGS__ ") = " << result << " - " <<\
		(result == expected ? "ok" : (++errors, "fail")) << "\n";\
}
#define DECLARE(...) std::cout << #__VA_ARGS__ "\n"; __VA_ARGS__
		std::size_t errors = 0;
	    EQ(false, 1, 2);
	    EQ(true, 2, 2);
	    //EQ(2, "hello"); //compiler error: 2.begin() not found
	    //EQ("hello", 3); //compiler error: 3.begin() not found
	    EQ(false, 2, 2.2);
	    EQ(false, 'd', 'e');
	    EQ(true, "hello", "hello");
	    DECLARE(int a[] = {1, 2, 3};)
	    DECLARE(int b[] = {1, 2, 3};)
	    EQ(true, a, b);
	    DECLARE(int c[] = {1, 2};)
	    //EQ(a, c); //linker error
	    EQ(false, {1, 2, 3}, c);
	    DECLARE(const char* x1 = "world";)
	    EQ(true, x1, "world");
	    DECLARE(const char x2[] = "world";)
	    EQ(true, x2, "world");
	    EQ(true, x2, x1);
	    EQ(true, {1, 2, 3}, {1, 2, 3});
	    EQ(false, {1, 2, 3}, {1, 2});
	    EQ(false, {1, 2, 3}, {1, 2, 4});
	    EQ(true, a, {1, 2, 3});
	    EQ(true, {1, 2, 3}, a);
	    EQ(false, a, {1, 2, 3, 4});

	    //1 - array-array
	    //DECLARE(int aa0[0];);
	    //DECLARE(double bb0[] = {};)
	    //EQ(true, aa0, bb0);
	    //EQ(true, bb0, aa0);
	    DECLARE(int aa1[1] = {1};);
	    DECLARE(double bb1[] = {1.0};)
	    DECLARE(int aa2[] = {3, 4};);
	    DECLARE(double bb2[] = {3.0, 4.0};)
	    EQ(true, aa1, bb1);
	    EQ(true, bb1, aa1);
	    EQ(true, aa2, bb2);
	    EQ(true, bb2, aa2);
	    //EQ(true, aa1, aa2); //compilation error
	    DECLARE(char const* cc2[] = {"3", "4"});
	    EQ(true, cc2, cc2);
	    //EQ(false, aa2, cc2); //comilation error
	    DECLARE(double dd1[] = {10.0};)
	    DECLARE(double dd2[] = {11.0, 12.0};)
	    EQ(false, aa1, dd1);
	    EQ(false, dd1, aa1);
	    EQ(false, aa2, dd2);
	    EQ(false, dd2, aa2);
	    DECLARE(int xx[2][3] = {{1, 2, 3}, {4, 5, 6}});
	    DECLARE(int yy[2][3] = {{1, 2, 3}, {4, 5, 6}});
	    EQ(true, xx, yy);
	    EQ(true, yy, xx);
	    DECLARE(int zz[2][3] = {{1, 2, 3}, {4, 5, 7}});
	    EQ(false, xx, zz);
	    EQ(false, zz, xx);
	    EQ(false, yy, zz);
	    EQ(false, zz, yy);
		DECLARE(int qq[][2][3][4] =
		{
				{{{15, 16, 17, 18}, {19, 20, 21, 22}, {23, 24, 25, 26}}, {{27, 28, 29, 30}, {31, 32, 33, 34}, {35, 36, 37, 38}}},
				{{{39, 40, 41, 42}, {43, 44, 45, 46}, {47, 48, 49, 50}}, {{51, 52, 53, 54}, {55, 56, 57, 58}, {59, 60, 61, 62}}},
		});
		DECLARE(int ww[][2][3][4] =
		{
				{{{15, 16, 17, 18}, {19, 20, 21, 22}, {23, 24, 25, 26}}, {{27, 28, 29, 30}, {31, 32, 33, 34}, {35, 36, 37, 38}}},
				{{{39, 40, 41, 42}, {43, 44, 45, 46}, {47, 48, 49, 50}}, {{51, 52, 53, 54}, {55, 56, 57, 58}, {59, 60, 61, 62}}},
		});
	    EQ(true, qq, ww);
	    EQ(true, ww, qq);
		DECLARE(int vv[][2][3][4] =
		{
				{{{15, 16, 17, 18}, {19, 20, 21, 22}, {23, 24, 25, 26}}, {{27, 28, 29, 30}, {31, 32, 33, 34}, {35, 36, 37, 38}}},
				{{{39, 40, 41, 42}, {43, 44, 45, 46}, {47, 48, 49, 50}}, {{51, 52, 53, 54}, {55, 56, 57, 58}, {59, 60, 61, 63}}},
		});
	    EQ(false, qq, vv);
	    EQ(false, vv, qq);
	    EQ(false, ww, vv);
	    EQ(false, vv, ww);
	    EQ(true, xx, xx);
	    EQ(true, yy, yy);
	    EQ(true, zz, zz);
	    EQ(true, qq, qq);
	    EQ(true, ww, ww);
	    EQ(true, vv, vv);
	    //EQ(false, xx, ww); //compilation error

	    //2

	    //3

	    //4 - skipped

	    //5

	    //6

	    //7

	    //8

	    //9 - skipped

	    //10

	    //11

	    //12

	    //13 - skipped

	    //14

	    //15

	    //16 - skipped

	    //17 - skipped

	    //18 - skipped

	    //19

	    //20

	    //21

	    std::cout << errors << " failures";
	}
#undef DECLARE
#undef EQ
}; //class EqualExample

#endif /* EXAMPLE_UT_EXAMPLE_HPP_ */
