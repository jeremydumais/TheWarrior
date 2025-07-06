#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "toolbarStateInfo.hpp"
#include "toolbarStateInfoJSONSerializer.hpp"
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using namespace std::literals;

namespace mapeditor::controllers::toolbarstateinfojsonserializer::unittest {

std::string ptree_toString(ptree node) {
    std::stringstream ss;
    write_json(ss, node, false);
    return ss.str();
}

ptree string_toPTree(const std::string &value) {
    ptree retval;
    std::stringstream ss(value);
    read_json(ss, retval);
    return retval;
}

std::string getOneDockedToolbarString() {
    return R"({"":{"id":"TextureSelection","isVisible":"true","dockLocation":"Left",)"s
    R"("vIndex":"0","width":"400","height":"200"}})"s + "\n"s;
}

std::string getOneNotDockedToolbarString() {
    return R"({"":{"id":"TextureSelection","isVisible":"true","dockLocation":"None",)"s
    R"("left":"200","top":"100","width":"400","height":"500"}})"s + "\n"s;
}

std::string getTwoToolbarsStringOneDockedOneNot() {
    return
    // First toolbar
    R"({"":{"id":"DebuggingInfo","isVisible":"false","dockLocation":"None",)"s
    R"("left":"200","top":"100","width":"400","height":"500"},)"s
    // Second toolbar
    R"("":{"id":"MapProperties","isVisible":"true","dockLocation":"Right",)"s
    R"("vIndex":"0","width":"400","height":"200"}})"s + "\n"s;
}

TEST(ToolBarStateInfoJSONSerializer_Serialize, WithEmptyVector_ReturnEmptyTree) {
    ASSERT_EQ("{}\n", ptree_toString(ToolBarStateInfoJSONSerializer::serialize({})));
}

TEST(ToolBarStateInfoJSONSerializer_Serialize, WithOneDockedToolBar_ReturnOneSerializedToolBar) {
    std::string expected = getOneDockedToolbarString();
    ASSERT_EQ(expected, ptree_toString(ToolBarStateInfoJSONSerializer::serialize({{
                    ToolBarId::TextureSelection,
                    true,
                    DockLocation::Left,
                    0,
                    0,
                    0,
                    400,
                    200
                    }})));
}

TEST(ToolBarStateInfoJSONSerializer_Serialize, WithOneNotDockedToolBar_ReturnOneSerializedToolBar) {
    std::string expected = getOneNotDockedToolbarString();
    ASSERT_EQ(expected, ptree_toString(ToolBarStateInfoJSONSerializer::serialize({{
                    ToolBarId::TextureSelection,
                    true,
                    DockLocation::None,
                    0,
                    200,
                    100,
                    400,
                    500
                    }})));
}

TEST(ToolBarStateInfoJSONSerializer_Serialize, WithTwoToolBarsOneDockedOneNot_ReturnTwoSerializedToolBars) {
    std::string expected = getTwoToolbarsStringOneDockedOneNot();
    ASSERT_EQ(expected, ptree_toString(ToolBarStateInfoJSONSerializer::serialize({
                    {
                        ToolBarId::DebuggingInfo,
                        false,
                        DockLocation::None,
                        0,
                        200,
                        100,
                        400,
                        500
                    },
                    {
                        ToolBarId::MapProperties,
                        true,
                        DockLocation::Right,
                        0,
                        0,
                        0,
                        400,
                        200
                    }
                    })));
}

TEST(ToolBarStateInfoJSONSerializer_Deserialize, WithEmptyPTree_ReturnEmptyVector) {
    auto node = string_toPTree("{}\n");
    auto actual = ToolBarStateInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(0, actual.size());
}

TEST(ToolBarStateInfoJSONSerializer_Deserialize, WithOneDockedToolBar_ReturnOneToolBar) {
    auto node = string_toPTree(getOneDockedToolbarString());
    auto actual = ToolBarStateInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(ToolBarId::TextureSelection, actual[0].id);
    ASSERT_TRUE(actual[0].isVisible);
    ASSERT_EQ(DockLocation::Left, actual[0].dockLocation);
    ASSERT_EQ(0, actual[0].vIndex);
    ASSERT_EQ(0, actual[0].left);
    ASSERT_EQ(0, actual[0].top);
    ASSERT_EQ(400, actual[0].width);
    ASSERT_EQ(200, actual[0].height);
}

TEST(ToolBarStateInfoJSONSerializer_Deserialize, WithOneNotDockedToolBar_ReturnOneToolBar) {
    auto node = string_toPTree(getOneNotDockedToolbarString());
    auto actual = ToolBarStateInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ(ToolBarId::TextureSelection, actual[0].id);
    ASSERT_TRUE(actual[0].isVisible);
    ASSERT_EQ(DockLocation::None, actual[0].dockLocation);
    ASSERT_EQ(0, actual[0].vIndex);
    ASSERT_EQ(200, actual[0].left);
    ASSERT_EQ(100, actual[0].top);
    ASSERT_EQ(400, actual[0].width);
    ASSERT_EQ(500, actual[0].height);
}

TEST(ToolBarStateInfoJSONSerializer_Deserialize, WithTwoToolBars_ReturnTwoToolBars) {
    auto node = string_toPTree(getTwoToolbarsStringOneDockedOneNot());
    auto actual = ToolBarStateInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ(ToolBarId::DebuggingInfo, actual[0].id);
    ASSERT_FALSE(actual[0].isVisible);
    ASSERT_EQ(DockLocation::None, actual[0].dockLocation);
    ASSERT_EQ(0, actual[0].vIndex);
    ASSERT_EQ(200, actual[0].left);
    ASSERT_EQ(100, actual[0].top);
    ASSERT_EQ(400, actual[0].width);
    ASSERT_EQ(500, actual[0].height);
    ASSERT_EQ(ToolBarId::MapProperties, actual[1].id);
    ASSERT_TRUE(actual[1].isVisible);
    ASSERT_EQ(DockLocation::Right, actual[1].dockLocation);
    ASSERT_EQ(0, actual[1].vIndex);
    ASSERT_EQ(0, actual[1].left);
    ASSERT_EQ(0, actual[1].top);
    ASSERT_EQ(400, actual[1].width);
    ASSERT_EQ(200, actual[1].height);
}

}  // namespace mapeditor::controllers::toolbarstateinfojsonserializer::unittest
