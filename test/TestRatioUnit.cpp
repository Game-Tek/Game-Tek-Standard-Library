#include <gtest/gtest.h>

#include "GTSL/RatioUnit.hpp"
#include "GTSL/DataSizes.h"

TEST(RatioUnit, Build) {
    GTSL::MegaByte mb(1);

    GTEST_ASSERT_EQ(mb, GTSL::MegaByte(1));

    GTSL::Byte b(mb);

    GTEST_ASSERT_EQ(b, GTSL::Byte(1 * 1024 * 1024));
}