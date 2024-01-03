#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include "math.hpp"
#include "geometry.hpp"
#include "shapes.hpp"
#include <gtest/gtest.h>

#ifndef WORKDIR
#define WORKDIR "../../"
#endif

// GTEST Utilities
inline char const* test_name()
{
    return ::testing::UnitTest::GetInstance()->current_test_info()->name();
}

namespace testing::internal {
    AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, glm::vec2 const& val1, glm::vec2 const& val2, double abs_error);
    AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, glm::vec3 const& val1, glm::vec3 const& val2, double abs_error);
    AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, aabb const& val1, aabb const& val2, double abs_error);
    AssertionResult DoubleNearPredFormat(const char* expr1, const char* expr2, const char* abs_error_expr, triangle const& val1, triangle const& val2, double abs_error);
}


#endif
