#include "manageMonsterController.hpp"
#include <gtest/gtest.h>
#include <memory>

class MonsterDTOSamples
{
public:
    static MonsterDTO getSample1() { 
        MonsterDTO retVal;
        retVal.id = "mon001";
        retVal.name = "monster1";
        retVal.textureName = "tex1";
        retVal.textureIndex = 0;
        return retVal;
    };
    static MonsterDTO getSample2() { 
        MonsterDTO retVal;
        retVal.id = "mon002";
        retVal.name = "monster2";
        retVal.textureName = "tex2";
        retVal.textureIndex = 1;
        return retVal;
    };
};

class MonsterStoreWith2Monsters : public ::testing::Test
{
public:
    MonsterStoreWith2Monsters() 
        : m_monsterStore(std::make_shared<MonsterStore>()),
          m_controller(m_monsterStore)
    {
        m_controller.addMonster(std::make_unique<MonsterDTO>(MonsterDTOSamples::getSample1()));
        m_controller.addMonster(std::make_unique<MonsterDTO>(MonsterDTOSamples::getSample2()));
    }
    std::shared_ptr<MonsterStore> m_monsterStore;
    ManageMonsterController m_controller;

};

TEST_F(MonsterStoreWith2Monsters, getMonster_WithEmptyId_ReturnNullPtr)
{
    ASSERT_EQ(nullptr, m_controller.getMonster(""));
}

TEST_F(MonsterStoreWith2Monsters, getMonster_WithWhiteSpacesId_ReturnNullPtr)
{
    ASSERT_EQ(nullptr, m_controller.getMonster("   "));
}

TEST_F(MonsterStoreWith2Monsters, getMonster_WithInexistantId_ReturnNullPtr)
{
    ASSERT_EQ(nullptr, m_controller.getMonster("eee444"));
}

TEST_F(MonsterStoreWith2Monsters, getMonster_WithIte001_ReturnMonsterDTO)
{
    auto actual = m_controller.getMonster("mon001");
    ASSERT_NE(nullptr, actual);
    ASSERT_EQ("mon001", actual->id);
    ASSERT_EQ("monster1", actual->name);
    ASSERT_EQ("tex1", actual->textureName);
    ASSERT_EQ(0, actual->textureIndex);
}

TEST(ManageMonsterController_addMonster, WithEmptyStore_ReturnTrue)
{
    auto store = std::make_shared<MonsterStore>();
    ManageMonsterController controller(store);
    ASSERT_TRUE(controller.addMonster(std::make_unique<MonsterDTO>(MonsterDTOSamples::getSample1())));
    ASSERT_EQ(1, store->getMonsterCount());
}

TEST(ManageMonsterController_addMonster, WithInvalidMonsterCreationInfo_ReturnFalse)
{
    auto store = std::make_shared<MonsterStore>();
    ManageMonsterController controller(store);
    auto monster = std::make_unique<MonsterDTO>();
    monster->id = "a";
    monster->name = "b";
    monster->textureName = "c";
    monster->textureIndex = 0;
    ASSERT_FALSE(controller.addMonster(std::move(monster)));
    ASSERT_EQ("id must be 6 characters long.", controller.getLastError());
}

TEST_F(MonsterStoreWith2Monsters, addMonster_WithExistingId_ReturnFalse)
{
    ASSERT_FALSE(m_controller.addMonster(std::make_unique<MonsterDTO>(MonsterDTOSamples::getSample1())));
    ASSERT_EQ("Monster mon001 already exist in the store.", m_controller.getLastError());
}

TEST_F(MonsterStoreWith2Monsters, updateMonster_WithNoMonsterDTO_ReturnFalse)
{
    ASSERT_FALSE(m_controller.updateMonster(nullptr, "mon001"));
    ASSERT_EQ("No monsterInfo structure has been provided.", m_controller.getLastError());
}

TEST_F(MonsterStoreWith2Monsters, updateMonster_WithExistingId_ReturnFalse)
{
    auto sampleWithSameId = MonsterDTOSamples::getSample1();
    auto originalId = sampleWithSameId.id;
    sampleWithSameId.id = MonsterDTOSamples::getSample2().id;
    ASSERT_FALSE(m_controller.updateMonster(std::make_unique<MonsterDTO>(sampleWithSameId), originalId));
    ASSERT_EQ("Monster mon002 already exist in the store.", m_controller.getLastError());
}

TEST_F(MonsterStoreWith2Monsters, updateMonster_WithNonExistingId_ReturnTrue)
{
    auto sampleWithSameId = MonsterDTOSamples::getSample1();
    auto originalId = sampleWithSameId.id;
    auto newId = "tes111";
    sampleWithSameId.id = newId;
    ASSERT_TRUE(m_controller.updateMonster(std::make_unique<MonsterDTO>(sampleWithSameId), originalId));
    auto actual = m_controller.getMonster(newId);
    ASSERT_EQ(newId, actual->id);
    ASSERT_EQ("monster1", actual->name);
    ASSERT_EQ("tex1", actual->textureName);
    ASSERT_EQ(0, actual->textureIndex);
}
