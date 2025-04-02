#include <gtest/gtest.h>
#include "../imageLoader.hpp"

/* Image Loader Test*/
TEST(ImageLoader, TestImageloader) {
    int imageWidth = 28;
    int imageLength = 28;
    auto imageSet = ImageLoader::LoadImages(imageWidth, imageLength);
    ASSERT_TRUE(1);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}
