#include <gtest/gtest.h>
#include "glComponentController.hpp"
#include "mainController.hpp"

using mapeditor::controllers::GLComponentController;
using mapeditor::controllers::MainController;

class SampleGLComponentController : public ::testing::Test {
 public:
    SampleGLComponentController() {
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        map->addTexture({
                "tex1",
                "tex1.png",
                512, 256,
                32, 32
                });
        map->addTexture({
                "tex2",
                "tex2.png",
                1024, 512,
                32, 32
                });
        auto &tile { map->getTileForEditing(0) };
        tile.setTextureName("tex1");
        tile.setTextureIndex(0);
        tile.setObjectTextureName("tex1");
        tile.setObjectTextureIndex(0);
        glComponentController.setCurrentMap(map);
    }
    MainController mainController;
    GLComponentController glComponentController;
};


// N = Not assigned, A = Assigned
//      0 1 2 3 4 5
//      -----------
//   0  N N N N N N
//   6  N N A N N N
//  12  N A A A N N
//  18  N N A N N N
//  24  N N N N A N
//  30  N N N N N N
class SampleGLComponentControllerWithTilesAssigned : public ::testing::Test {
 public:
    SampleGLComponentControllerWithTilesAssigned() {
        mainController.createMap(6, 6);
        auto map { mainController.getMap() };
        map->addTexture({
                "tex1",
                "tex1.png",
                512, 256,
                32, 32
                });
        auto &tile8 { map->getTileForEditing(8) };
        tile8.setTextureName("tex1");
        tile8.setTextureIndex(0);
        auto &tile13 { map->getTileForEditing(13) };
        tile13.setTextureIndex(0);
        auto &tile14 { map->getTileForEditing(14) };
        tile14.setTextureIndex(0);
        auto &tile15 { map->getTileForEditing(15) };
        tile15.setTextureIndex(1);
        auto &tile20 { map->getTileForEditing(20) };
        tile20.setObjectTextureIndex(1);
        auto &tile28 { map->getTileForEditing(28) };
        tile28.setObjectTextureName("tex1");
        glComponentController.setCurrentMap(map);
    }
    MainController mainController;
    GLComponentController glComponentController;
};

TEST(GLComponentController_getAlreadyUsedTextureNames, DefaultConstructor_ReturnEmptyVector) {
    GLComponentController glComponentController;
    ASSERT_EQ(0, glComponentController.getAlreadyUsedTextureNames().size());
}

TEST_F(SampleGLComponentController, getAlreadyUsedTextureNames_WithTwoTexture_ReturnVectorSizeTeo) {
    ASSERT_EQ(2, glComponentController.getAlreadyUsedTextureNames().size());
}

TEST_F(SampleGLComponentController, isTextureUsedInMap_WithUnusedTexture_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isTextureUsedInMap("x"));
}

TEST_F(SampleGLComponentController, isTextureUsedInMap_WithUsedTexture_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isTextureUsedInMap("tex1"));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnLeft_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(-1, 0, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnLeft_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(-2, 0, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnTop_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(0, -1, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnTop_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(0, -2, 0, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnRight_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, -1, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnRight_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, -2, 0));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusOneOnBottom_ReturnFalse) {
    ASSERT_FALSE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, 0, -1));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, isShrinkMapImpactAssignedTiles_WithMinusTwoOnBottom_ReturnTrue) {
    ASSERT_TRUE(glComponentController.isShrinkMapImpactAssignedTiles(0, 0, 0, -2));
}

TEST_F(SampleGLComponentControllerWithTilesAssigned, resizeMap_WithMinusOneOnLeft_ReturnSuccess) {
    glComponentController.resizeMap(-1, 0, 0, 0);
    ASSERT_EQ(5, glComponentController.getMap()->getWidth());
    ASSERT_EQ(6, glComponentController.getMap()->getHeight());
}
