#include <gtest/gtest.h>
#include "editMonsterZoneFormController.hpp"
#include "containerOfMonsterStoreTestUtils.hpp"
#include "monsterZoneMonsterEncounter.hpp"
#include <stdexcept>

using namespace mapeditor::controllers::test;
using namespace thewarrior::models;

namespace mapeditor::controllers::editmonsterzoneformcontroller::unittest {

class EditMonsterZoneFormControllerWith2StoresSample : public ::testing::Test
{
public:
    EditMonsterZoneFormControllerWith2StoresSample()
    {

        controller = std::make_shared<EditMonsterZoneFormController>(ContainerOfMonsterStoreTestUtils::getMonsterStores2Samples(),
                                                                     "fakeResourcesPath");
    }
    std::shared_ptr<EditMonsterZoneFormController> controller = nullptr;
    std::vector<std::string> alreadyUsedMonsterIds = { "SLM001", "DRA001" };
};

TEST_F(EditMonsterZoneFormControllerWith2StoresSample, getResourcesPath_ReturnFakeResourcesPath)
{
    ASSERT_EQ("fakeResourcesPath", controller->getResourcesPath());
}

} // namespace mapeditor::controllers::editmonsterzoneformcontroller
