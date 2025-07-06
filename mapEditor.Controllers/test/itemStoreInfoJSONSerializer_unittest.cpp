#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "itemStoreInfoJSONSerializer.hpp"
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using namespace std::literals;

namespace mapeditor::controllers::itemstoreinfojsonserializer::unittest {

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

std::string getOneItemString() {
    return R"({"":{"name":"disp1","filename":"filename1"}})"s + "\n"s;
}

std::string getTwoItemString() {
    return R"({"":{"name":"d1","filename":"f1"},"":{"name":"d2","filename":"f2"}})"s + "\n"s;
}

TEST(ItemStoreInfoJSONSerializer_Serialize, WithEmptyVector_ReturnEmptyTree) {
    ASSERT_EQ("{}\n", ptree_toString(ItemStoreInfoJSONSerializer::serialize({})));
}

TEST(ItemStoreInfoJSONSerializer_Serialize, WithOneItemStoreInfo_ReturnOneItemTree) {
    std::string expected = getOneItemString();
    ASSERT_EQ(expected, ptree_toString(ItemStoreInfoJSONSerializer::serialize({
                    { .name = "disp1", .filename = "filename1" }
                    })));
}

TEST(ItemStoreInfoJSONSerializer_Serialize, WithTwoItemStoreInfos_ReturnTwoItemTree) {
    std::string expected = getTwoItemString();
    ASSERT_EQ(expected, ptree_toString(ItemStoreInfoJSONSerializer::serialize({
                    { .name = "d1", .filename = "f1" },
                    { .name = "d2", .filename = "f2" }
                    })));
}

TEST(ItemStoreInfoJSONSerializer_Deserialize, WithEmptyPTree_ReturnEmptyVector) {
    auto node = string_toPTree("{}\n");
    auto actual = ItemStoreInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(0, actual.size());
}

TEST(ItemStoreInfoJSONSerializer_Deserialize, WithOneItem_ReturnOneItemVector) {
    auto node = string_toPTree(getOneItemString());
    auto actual = ItemStoreInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ("disp1", actual[0].name);
    ASSERT_EQ("filename1", actual[0].filename);
}

TEST(ItemStoreInfoJSONSerializer_Deserialize, WithTwoItems_ReturnTwoItemsVector) {
    auto node = string_toPTree(getTwoItemString());
    auto actual = ItemStoreInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("d1", actual[0].name);
    ASSERT_EQ("f1", actual[0].filename);
    ASSERT_EQ("d2", actual[1].name);
    ASSERT_EQ("f2", actual[1].filename);
}

}  // namespace mapeditor::controllers::itemstoreinfojsonserializer::unittest
