#define BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "monsterStoreInfoJSONSerializer.hpp"
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using namespace std::literals;

namespace mapeditor::controllers::monsterstoreinfojsonserializer::unittest {

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

std::string getOneMonsterStoreString() {
    return R"({"":{"name":"disp1","filename":"filename1"}})"s + "\n"s;
}

std::string getTwoMonsterStoresString() {
    return R"({"":{"name":"d1","filename":"f1"},"":{"name":"d2","filename":"f2"}})"s + "\n"s;
}

TEST(MonsterStoreInfoJSONSerializer_Serialize, WithEmptyVector_ReturnEmptyTree) {
    ASSERT_EQ("{}\n", ptree_toString(MonsterStoreInfoJSONSerializer::serialize({})));
}

TEST(MonsterStoreInfoJSONSerializer_Serialize, WithOneMonsterStoreInfo_ReturnOneMonsterTree) {
    std::string expected = getOneMonsterStoreString();
    ASSERT_EQ(expected, ptree_toString(MonsterStoreInfoJSONSerializer::serialize({
                    { .name = "disp1", .filename = "filename1" }
                    })));
}

TEST(MonsterStoreInfoJSONSerializer_Serialize, WithTwoMonsterStoreInfos_ReturnTwoMonsterTree) {
    std::string expected = getTwoMonsterStoresString();
    ASSERT_EQ(expected, ptree_toString(MonsterStoreInfoJSONSerializer::serialize({
                    { .name = "d1", .filename = "f1" },
                    { .name = "d2", .filename = "f2" }
                    })));
}

TEST(MonsterStoreInfoJSONSerializer_Deserialize, WithEmptyPTree_ReturnEmptyVector) {
    auto node = string_toPTree("{}\n");
    auto actual = MonsterStoreInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(0, actual.size());
}

TEST(MonsterStoreInfoJSONSerializer_Deserialize, WithOneMonster_ReturnOneMonsterVector) {
    auto node = string_toPTree(getOneMonsterStoreString());
    auto actual = MonsterStoreInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(1, actual.size());
    ASSERT_EQ("disp1", actual[0].name);
    ASSERT_EQ("filename1", actual[0].filename);
}

TEST(MonsterStoreInfoJSONSerializer_Deserialize, WithTwoMonsters_ReturnTwoMonstersVector) {
    auto node = string_toPTree(getTwoMonsterStoresString());
    auto actual = MonsterStoreInfoJSONSerializer::deserialize(node);
    ASSERT_EQ(2, actual.size());
    ASSERT_EQ("d1", actual[0].name);
    ASSERT_EQ("f1", actual[0].filename);
    ASSERT_EQ("d2", actual[1].name);
    ASSERT_EQ("f2", actual[1].filename);
}

}  // namespace mapeditor::controllers::monsterstoreinfojsonserializer::unittest
