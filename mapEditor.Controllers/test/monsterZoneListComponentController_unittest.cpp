#include <gmock/gmock-function-mocker.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdlib.h>
#include <memory>
#include <string>
#include <vector>
#include "../include/monsterZoneListComponentController.hpp"
#include "glComponentController.hpp"

using ::testing::Return;

namespace mapeditor::controllers::monsterzonelistcomponentcontroller::unittest {

class MockGLComponentController : public GLComponentController {
 public:
     MOCK_METHOD(std::vector<std::string>, getAlreadyUsedMonsterZoneNames, (), (const, override));
};

TEST(monsterZoneListComponentController_getAlreadyUsedMonsterZoneNamesForEdition,
       WithEmptyVector_ReturnEmptyVector) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getAlreadyUsedMonsterZoneNames())
         .WillByDefault(Return(std::vector<std::string>({})));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_EQ(0, controller.getAlreadyUsedMonsterZoneNames().size());
}

TEST(monsterZoneListComponentController_getAlreadyUsedMonsterZoneNamesForEdition,
       With2Zones_Return2ZonesVector) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getAlreadyUsedMonsterZoneNames())
         .WillByDefault(Return(std::vector<std::string>({"1", "2"})));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_EQ(2, controller.getAlreadyUsedMonsterZoneNames().size());
     ASSERT_EQ("1", controller.getAlreadyUsedMonsterZoneNames()[0]);
     ASSERT_EQ("2", controller.getAlreadyUsedMonsterZoneNames()[1]);
}

}  // namespace mapeditor::controllers::monsterzonelistcomponentcontroller::unittest

