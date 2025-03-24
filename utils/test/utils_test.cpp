#include <gtest/gtest.h>
#include "../imageLoader.hpp"

/* Protocol Init Tests */
TEST(ImageLoader, TestImageloader) {
    std::vector<std::vector<unsigned char>> imageSet = Utils::LoadImages();
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}
