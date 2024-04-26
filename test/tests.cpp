#include <gtest/gtest.h>

// there are no any tests for this code
TEST(Empty, Test) 
{
    // Arrange
    bool test;

    // Act
	test = true;

    // Assert
    ASSERT_EQ(test, true);
}

int main(int argc, char** argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
