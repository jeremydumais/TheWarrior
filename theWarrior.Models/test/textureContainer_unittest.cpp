#include "textureContainer.hpp""
#include <gtest/gtest.h>

using namespace std;

struct DefaultConstructedTextureContainer : public ::testing::Test
{
TextureContainer container;
};

TEST_F(DefaultConstructedTextureContainer, ReturnSuccess)
{
    ASSERT_EQ(0, container.getCount());
}

