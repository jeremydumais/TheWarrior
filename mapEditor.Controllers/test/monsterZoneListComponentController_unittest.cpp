#include <gmock/gmock-function-mocker.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/monsterZoneListComponentController.hpp"
#include "glComponentController.hpp"
#include <memory>
#include <stdlib.h>
#include <vector>

using ::testing::Return;
using ::testing::NiceMock;
using ::testing::_;

namespace mapeditor::controllers::monsterzonelistcomponentcontroller::unittest {

class MockGLComponentController : public GLComponentController {
 public:
     std::vector<std::string> getAlreadyUsedMonsterZoneNames() {
         return { "A", "B", "C" };
     }
     //MOCK_METHOD(std::vector<std::string>, getAlreadyUsedMonsterZoneNames, (), (const));
};


TEST(monsterZoneListComponentController_getAlreadyUsedMonsterZoneNamesForEdition,
       WithEmptyVector_ReturnEmptyVector) {
    //TODO: Complete this test...
    //auto mockGLController = std::make_shared<MockGLComponentController>();
    //MonsterZoneListComponentController controller(mockGLController.get());
    //ASSERT_EQ(2, controller.getAlreadyUsedMonsterZoneNames().size());
}

} // namespace mapeditor::controllers::monsterzonelistcomponentcontroller::unittest {

