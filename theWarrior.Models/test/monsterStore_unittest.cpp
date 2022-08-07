#include "monsterStore.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace thewarrior::models;

const Monster monsterSample1({ "slm001", "Blue Slime", "tex1", 1, 15, 1.1F, 1.2F, 3, 4});
const Monster monsterSample2({ "slm002", "Red Slime", "tex1", 2, 20, 2.1F, 2.2F, 5, 7});

struct DefaultConstructedMonsterStore : public ::testing::Test
{
    MonsterStore monsterStore;
};

struct MonsterStoreWithTwoMonsters : public ::testing::Test
{
    MonsterStoreWithTwoMonsters()
    {
        monsterStore.addMonster(std::make_shared<Monster>(monsterSample1));
        monsterStore.addMonster(std::make_shared<Monster>(monsterSample2));
    }
    MonsterStore monsterStore;
};

TEST_F(DefaultConstructedMonsterStore, ReturnSuccess)
{
    MonsterStore store;
    ASSERT_EQ(0, monsterStore.getMonsterCount());
}

TEST_F(DefaultConstructedMonsterStore, getMonsterCount_ReturnZero)
{
    ASSERT_EQ(0, monsterStore.getMonsterCount());
}

TEST_F(DefaultConstructedMonsterStore, getMonsters_ReturnEmpty)
{
    ASSERT_EQ(0, monsterStore.getMonsters().size());
}

TEST_F(MonsterStoreWithTwoMonsters, getMonsters_Return2Monsters)
{
    auto monsters = monsterStore.getMonsters();
    ASSERT_EQ(2, monsters.size());
    ASSERT_EQ("slm001", monsters[0]->getId());
    ASSERT_EQ("slm002", monsters[1]->getId());
}

TEST_F(MonsterStoreWithTwoMonsters, findMonster_NonExistingMonster_ReturnEmpty)
{
    auto result = monsterStore.findMonster("non123");
    ASSERT_TRUE(result == nullptr);
}

TEST_F(MonsterStoreWithTwoMonsters, findMonster_ExistingMonster1_ReturnMonster)
{
    auto result = monsterStore.findMonster("slm001");
    ASSERT_TRUE(result != nullptr);
    ASSERT_EQ(monsterSample1, *result);
}

TEST_F(MonsterStoreWithTwoMonsters, findMonster_ExistingMonster2_ReturnMonster)
{
    auto result = monsterStore.findMonster("slm002");
    ASSERT_TRUE(result != nullptr);
    ASSERT_EQ(monsterSample2, *result);
}

TEST_F(MonsterStoreWithTwoMonsters, isMonsterExists_NonExistingMonster_ReturnFalse)
{
    ASSERT_FALSE(monsterStore.isMonsterExists("non123"));
}

TEST_F(MonsterStoreWithTwoMonsters, isMonsterExists_ExistingMonster1_ReturnTrue)
{
    ASSERT_TRUE(monsterStore.isMonsterExists("slm001"));
}

TEST_F(MonsterStoreWithTwoMonsters, isMonsterExists_ExistingMonster2_ReturnTrue)
{
    ASSERT_TRUE(monsterStore.isMonsterExists("slm002"));
}

TEST_F(DefaultConstructedMonsterStore, addMonster_NonExistingMonster_ReturnTrue)
{
    ASSERT_TRUE(monsterStore.addMonster(std::make_shared<Monster>(MonsterCreationInfo { "slm001", "Blue Slime", "tex1", 1})));
}

TEST_F(MonsterStoreWithTwoMonsters, addMonster_ExistingMonster_ReturnFalse)
{
    ASSERT_FALSE(monsterStore.addMonster(std::make_shared<Monster>(MonsterCreationInfo { "slm001", "Blue Slime", "tex1", 1})));
}

TEST_F(MonsterStoreWithTwoMonsters, addMonster_NonExistingMonster_ReturnTrue)
{
    ASSERT_TRUE(monsterStore.addMonster(std::make_shared<Monster>(MonsterCreationInfo { "slm003", "Gray Slime", "tex2", 3})));
}

TEST_F(MonsterStoreWithTwoMonsters, replaceMonster_NonExistingMonster_ReturnFalse)
{
    ASSERT_FALSE(monsterStore.replaceMonster("abc123", std::make_shared<Monster>(MonsterCreationInfo { "fgh001", "Test", "tex1", 1})));
}

TEST_F(MonsterStoreWithTwoMonsters, replaceMonster_ExistingMonster_ReturnTrue)
{
    auto expected = std::make_shared<Monster>(MonsterCreationInfo { "fgh001", "Test", "tex1", 1});
    ASSERT_TRUE(monsterStore.replaceMonster("slm001", expected));
    ASSERT_EQ(2, monsterStore.getMonsterCount());
    ASSERT_EQ(*expected, *monsterStore.findMonster("fgh001"));
    ASSERT_TRUE(monsterStore.findMonster("slm001") == nullptr);
}

TEST_F(MonsterStoreWithTwoMonsters, replaceMonster_ExistingMonsterSameId_ReturnTrue)
{
    auto expected = std::make_shared<Monster>(MonsterCreationInfo { "slm001", "Test1", "tex10", 10});
    ASSERT_TRUE(monsterStore.replaceMonster("slm001", expected));
    ASSERT_EQ(2, monsterStore.getMonsterCount());
    ASSERT_EQ(*expected, *monsterStore.findMonster("slm001"));
}

TEST_F(MonsterStoreWithTwoMonsters, removeMonster_ExistingMonster_ReturnTrue)
{
    ASSERT_TRUE(monsterStore.removeMonster("slm001"));
}

TEST_F(MonsterStoreWithTwoMonsters, removeMonster_NonExistingMonster_ReturnFalse)
{
    ASSERT_FALSE(monsterStore.removeMonster("abc123"));
}
