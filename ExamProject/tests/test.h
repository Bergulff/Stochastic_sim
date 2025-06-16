// tests/test.h - Fixed minimal doctest implementation
#ifndef DOCTEST_LIBRARY_INCLUDED
#define DOCTEST_LIBRARY_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>
#include <fstream>

namespace doctest {
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };

    static std::vector<TestCase>& getTests() {
        static std::vector<TestCase> tests;
        return tests;
    }

    struct TestCaseRegister {
        TestCaseRegister(const char* name, std::function<void()> func) {
            getTests().push_back({name, func});
        }
    };

    struct Approx {
        double value;
        double epsilon;

        explicit Approx(double v) : value(v), epsilon(0.0001) {}

        friend bool operator==(double lhs, const Approx& rhs) {
            return std::abs(lhs - rhs.value) <= rhs.epsilon;
        }

        friend bool operator==(const Approx& lhs, double rhs) {
            return rhs == lhs;
        }
    };

    inline Approx Approx(double value) {
        return ::doctest::Approx(value);
    }

    // Global test counters
    static int& getPassedCount() {
        static int passed = 0;
        return passed;
    }

    static int& getFailedCount() {
        static int failed = 0;
        return failed;
    }
}

// Better macro concatenation
#define DOCTEST_CAT_IMPL(s1, s2) s1##s2
#define DOCTEST_CAT(s1, s2) DOCTEST_CAT_IMPL(s1, s2)
#define DOCTEST_ANONYMOUS(x) DOCTEST_CAT(x, __LINE__)

#define TEST_CASE(name) \
    static void DOCTEST_ANONYMOUS(test_function_)(); \
    static doctest::TestCaseRegister DOCTEST_ANONYMOUS(test_register_)(name, &DOCTEST_ANONYMOUS(test_function_)); \
    static void DOCTEST_ANONYMOUS(test_function_)()

#define SUBCASE(name) \
    std::cout << "  SUBCASE: " << name << std::endl;

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "CHECK FAILED: " << #expr << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            doctest::getFailedCount()++; \
        } else { \
            std::cout << "CHECK PASSED: " << #expr << std::endl; \
            doctest::getPassedCount()++; \
        } \
    } while(0)

#define CHECK_FALSE(expr) CHECK(!(expr))

#define CHECK_THROWS_AS(expr, exception_type) \
    do { \
        bool threw_correct = false; \
        try { \
            expr; \
        } catch (const exception_type&) { \
            threw_correct = true; \
        } catch (...) {} \
        if (threw_correct) { \
            std::cout << "CHECK_THROWS_AS PASSED: " << #expr << std::endl; \
            doctest::getPassedCount()++; \
        } else { \
            std::cerr << "CHECK_THROWS_AS FAILED: " << #expr << " did not throw " << #exception_type << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            doctest::getFailedCount()++; \
        } \
    } while(0)

#define CHECK_NOTHROW(expr) \
    do { \
        try { \
            expr; \
            std::cout << "CHECK_NOTHROW PASSED: " << #expr << std::endl; \
            doctest::getPassedCount()++; \
        } catch (...) { \
            std::cerr << "CHECK_NOTHROW FAILED: " << #expr << " threw an exception at " << __FILE__ << ":" << __LINE__ << std::endl; \
            doctest::getFailedCount()++; \
        } \
    } while(0)

#define MESSAGE(msg) std::cout << "MESSAGE: " << msg << std::endl

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
int main() {
    std::cout << "===========================================" << std::endl;
    std::cout << "Running doctest tests..." << std::endl;
    std::cout << "===========================================" << std::endl;

    int test_count = 0;
    int test_passed = 0;

    for (const auto& test : doctest::getTests()) {
        test_count++;
        std::cout << "\n[TEST " << test_count << "] " << test.name << std::endl;
        std::cout << "-------------------------------------------" << std::endl;

        int initial_passed = doctest::getPassedCount();
        int initial_failed = doctest::getFailedCount();

        try {
            test.func();
            test_passed++;
            std::cout << "✓ TEST PASSED: " << test.name << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "✗ TEST FAILED: " << test.name << " - Exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "✗ TEST FAILED: " << test.name << " - Unknown exception" << std::endl;
        }

        int test_passed_checks = doctest::getPassedCount() - initial_passed;
        int test_failed_checks = doctest::getFailedCount() - initial_failed;
        std::cout << "  Checks: " << test_passed_checks << " passed, " << test_failed_checks << " failed" << std::endl;
    }

    std::cout << "\n===========================================" << std::endl;
    std::cout << "SUMMARY:" << std::endl;
    std::cout << "Tests: " << test_passed << "/" << test_count << " passed" << std::endl;
    std::cout << "Checks: " << doctest::getPassedCount() << " passed, " << doctest::getFailedCount() << " failed" << std::endl;
    std::cout << "===========================================" << std::endl;

    return (test_count - test_passed) > 0 ? 1 : 0;
}
#endif

#endif // DOCTEST_LIBRARY_INCLUDED