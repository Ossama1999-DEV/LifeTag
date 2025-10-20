#include <gtest/gtest.h>
#include "version.h"

TEST(VersionTest, ReturnsCorrectVersion) {
    EXPECT_EQ(LifeTag::getVersion(), "LifeTag v0.1.0");
}

TEST(VersionTest, NotEmpty) {
    EXPECT_FALSE(LifeTag::getVersion().empty());
}

// Main gtest standard
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
