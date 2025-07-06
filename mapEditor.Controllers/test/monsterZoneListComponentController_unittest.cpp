#include <gmock/gmock-function-mocker.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "../include/monsterZoneListComponentController.hpp"
#include "glComponentController.hpp"
#include "monsterZoneDTO.hpp"

using ::testing::NiceMock;
using ::testing::Return;
using ::testing::_;
using mapeditor::controllers::MonsterZoneDTO;

namespace mapeditor::controllers::monsterzonelistcomponentcontroller::unittest {

class MockGLComponentController : public GLComponentController {
 public:
     MOCK_METHOD(std::vector<std::string>, getAlreadyUsedMonsterZoneNames, (), (const, override));
     MOCK_METHOD(std::vector<MonsterZoneDTO>, getMonsterZones, (), (const));
     MOCK_METHOD(OptMonsterZoneDTOConst, getMonsterZoneByName, (const std::string &name), (const));
};

TEST(monsterZoneListComponentController_getMonsterZones,
       WithNoZones_ReturnEmptyZonesVector) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getMonsterZones())
         .WillByDefault(Return(std::vector<MonsterZoneDTO>({
     })));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_EQ(0, controller.getMonsterZones().size());
}

TEST(monsterZoneListComponentController_getMonsterZones,
       With2Zones_Return2ZonesVector) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getMonsterZones())
         .WillByDefault(Return(std::vector<MonsterZoneDTO>({
            MonsterZoneDTO { .m_name = "Test1" },
            MonsterZoneDTO { .m_name = "Test2" }
     })));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_EQ(2, controller.getMonsterZones().size());
}

TEST(monsterZoneListComponentController_getMonsterZoneByName,
       WithZoneNameNotFound_ReturnEmptyResult) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getMonsterZoneByName(_))
         .WillByDefault(Return(std::nullopt));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_EQ(std::nullopt, controller.getMonsterZoneByName("Test1"));
}

TEST(monsterZoneListComponentController_getMonsterZoneByName,
       WithFoundZoneName_ReturnZone) {
     std::string zoneName("Test2");
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getMonsterZoneByName(_))
         .WillByDefault(Return(OptMonsterZoneDTOConst { MonsterZoneDTO { .m_name = zoneName }}));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     auto result = controller.getMonsterZoneByName(zoneName);
     ASSERT_TRUE(result.has_value());
     ASSERT_EQ(zoneName, result->m_name);
}

TEST(monsterZoneListComponentController_isMonsterZoneEmpty,
       With0Zone_ReturnTrue) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getMonsterZones())
         .WillByDefault(Return(std::vector<MonsterZoneDTO>()));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_TRUE(controller.isMonsterZonesEmpty());
}

TEST(monsterZoneListComponentController_isMonsterZoneEmpty,
       With1Zone_ReturnFalse) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getMonsterZones())
         .WillByDefault(Return(std::vector<MonsterZoneDTO>({
            MonsterZoneDTO { .m_name = "Test1" },
     })));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_FALSE(controller.isMonsterZonesEmpty());
}

TEST(monsterZoneListComponentController_getAlreadyUsedMonsterZoneNames,
       WithEmptyVector_ReturnEmptyVector) {
     auto mockGLController = std::make_shared<MockGLComponentController>();
     ON_CALL(*mockGLController, getAlreadyUsedMonsterZoneNames())
         .WillByDefault(Return(std::vector<std::string>({})));
     MonsterZoneListComponentController controller(reinterpret_cast<GLComponentController *>(mockGLController.get()));
     ASSERT_EQ(0, controller.getAlreadyUsedMonsterZoneNames().size());
}

TEST(monsterZoneListComponentController_getAlreadyUsedMonsterZoneNames,
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


