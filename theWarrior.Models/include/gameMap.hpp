#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>
#include "mapTile.hpp"
#include "merchantInventory.hpp"
#include "monsterZone.hpp"
#include "npc.hpp"
#include "point.hpp"
#include "texture.hpp"
#include "textureContainer.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

using OptMonsterZoneConstRef = std::optional<std::reference_wrapper<const MonsterZone>>;
using OptNPCConstRef = std::optional<std::reference_wrapper<const NPC>>;
using OptMerchantInventoryConstRef = std::optional<std::reference_wrapper<const MerchantInventory>>;

class GameMap {
 public:
    GameMap(unsigned int width, unsigned int height);
    const std::string &getLastError() const;
    const std::vector<std::vector<MapTile>> &getTiles() const;
    MapTile &getTileForEditing(int index);
    MapTile &getTileForEditing(Point<> coord);
    std::vector<MapTile *> getTilesForEditing(const std::set<int> &indices);
    const MapTile &getTileFromCoord(Point<> coord) const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    const std::string &getMusicFilename() const;
    Point<> getCoordFromTileIndex(int index) const;
    int getTileIndexFromCoord(Point<> coord) const;
    const std::vector<Texture> &getTextures() const;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
    bool canSteppedOnTile(Point<> playerCoord) const;
    bool useOnlyOneMonsterZone() const;
    const std::vector<MonsterZone> &getMonsterZones() const;
    OptMonsterZoneConstRef getMonsterZoneByName(const std::string &zoneName) const;
    OptMerchantInventoryConstRef getMerchantInventoryByName(const std::string &inventoryName) const;
    bool isShrinkMapImpactAssignedTiles(int offsetLeft,
            int offsetTop,
            int offsetRight,
            int offsetBottom) const;
    void resizeMap(int offsetLeft,
                   int offsetTop,
                   int offsetRight,
                   int offsetBottom);
    void setMusicFilename(const std::string &filename);
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    // Monster zone methods
    bool addMonsterZone(const MonsterZone &zone);
    bool replaceMonsterZone(const std::string &name, const MonsterZone &zone);
    bool removeMonsterZone(const std::string &name);
    bool setUseOnlyOneMonsterZone(bool value);
    void unassignMonsterZoneOnAllTiles(int zoneIndex);
    // NPC methods
    bool isTileUsedByNPC(const Point<size_t> &coord) const;
    bool isTilesIndicesUsedByNPC(const std::set<int> &indices) const;
    const std::vector<NPC> &getNPCs() const;
    OptNPCConstRef getNPCById(const std::string &id) const;
    bool addNPC(const NPC &npc);
    bool replaceNPC(const std::string &npcId, const NPC &npc);
    bool removeNPC(const std::string &npcId);
    bool addNPCWanderingZone(const std::string &npcId, const std::set<int> &selectedTilesIndices);
    bool removeNPCWanderingZone(const std::string &npcId, const std::set<int> &selectedTilesIndices);
    // Merchant Inventory methods
    const std::vector<MerchantInventory> &getMerchantInventories() const;
    bool addMerchantInventory(const MerchantInventory &inventory);
    bool replaceMerchantInventory(const std::string &name, const MerchantInventory &inventory);
    bool removeMerchantInventory(const std::string &name);

 private:
    friend class boost::serialization::access;
    std::string m_lastError;
    std::vector<std::vector<MapTile>> m_tiles;
    std::vector<MonsterZone> m_monsterZones;
    std::vector<NPC> m_npcs;
    std::vector<MerchantInventory> m_merchantInventories;
    TextureContainer m_textureContainer;
    bool m_useOnlyOneMonsterZone = false;
    std::string m_musicFilename;
    bool _isShrinkMapFromLeftImpactAssignedTiles(int offset) const;
    bool _isShrinkMapFromTopImpactAssignedTiles(int offset) const;
    bool _isShrinkMapFromRightImpactAssignedTiles(int offset) const;
    bool _isShrinkMapFromBottomImpactAssignedTiles(int offset) const;
    void _resizeMapFromLeft(int offset);
    void _resizeMapFromTop(int offset);
    void _resizeMapFromRight(int offset);
    void _resizeMapFromBottom(int offset);
    std::vector<MonsterZone>::iterator getMonsterZoneIterator(const std::string &name);
    std::vector<NPC>::iterator getNPCIterator(const std::string &npcId);
    std::vector<MerchantInventory>::iterator getMerchantInventoryIterator(const std::string &name);
    // Serialization methods
    template<class Archive>
    void save(Archive& ar, const unsigned int version) const {
        ar & m_tiles;
        ar & m_textureContainer;
        if (version > 1) {
            ar & m_monsterZones;
        }
        if (version > 2) {
            ar & m_useOnlyOneMonsterZone;
        }
        if (version > 3) {
            ar & m_npcs;
        }
        if (version > 4) {
            ar & m_musicFilename;
        }
        if (version > 5) {
            ar & m_merchantInventories;
        }
    }
    template<class Archive>
    void load(Archive& ar, const unsigned int version) {
        m_monsterZones.clear();
        m_useOnlyOneMonsterZone = false;
        m_npcs.clear();
        m_musicFilename.clear();

        ar & m_tiles;
        ar & m_textureContainer;

        if (version > 1) {
            ar & m_monsterZones;
        }
        if (version > 2) {
            ar & m_useOnlyOneMonsterZone;
        }
        if (version > 3) {
            ar & m_npcs;
        }
        if (version > 4) {
            ar & m_musicFilename;
        }
        if (version > 5) {
            ar & m_merchantInventories;
        }
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::GameMap, 6)
