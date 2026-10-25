#include "glTextureService.hpp"
#include <gtest/gtest.h>

using thewarrior::models::Texture;
using thewarrior::ui::GLTextureService;

TEST(GLTextureService, PreservesCompleteTileAndSharedEdges) {
    const Texture texture({"atlas", "atlas.png", 256, 128, 64, 32});
    const GLTextureService service;
    GLfloat first[4][2], next[4][2], below[4][2], last[4][2];
    service.setTextureUVFromIndex(&texture, first, 0);
    service.setTextureUVFromIndex(&texture, next, 1);
    service.setTextureUVFromIndex(&texture, below, 4);
    service.setTextureUVFromIndex(&texture, last, 15);

    EXPECT_FLOAT_EQ(first[0][0], 0.0f);
    EXPECT_FLOAT_EQ(first[0][1], 0.75f);
    EXPECT_FLOAT_EQ(first[2][0], 0.25f);
    EXPECT_FLOAT_EQ(first[2][1], 1.0f);
    EXPECT_FLOAT_EQ(first[1][0], next[0][0]);
    EXPECT_FLOAT_EQ(first[0][1], below[3][1]);
    EXPECT_FLOAT_EQ(last[0][0], 0.75f);
    EXPECT_FLOAT_EQ(last[0][1], 0.0f);
    EXPECT_FLOAT_EQ(last[2][0], 1.0f);
    EXPECT_FLOAT_EQ(last[2][1], 0.25f);
}
