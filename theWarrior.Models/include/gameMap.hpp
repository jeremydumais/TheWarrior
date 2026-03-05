#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>
#include "mapTile.hpp"
#include "monsterZone.hpp"
#include "npc.hpp"
#include "point.hpp"
#include "texture.hpp"
#include "textureContainer.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace thewarrior::models {

typedef std::optional<std::reference_wrapper<const MonsterZone>> OptMonsterZoneConstRef;
typedef std::optional<std::reference_wrapper<const NPC>> OptNPCConstRef;

class GameMap {
 public:
    GameMap(unsigned int width, unsigned int height);
    const std::string &getLastError() const;
    const std::vector<std::vector<MapTile>> &getTiles() const;
    MapTile &getTileForEditing(int index);
    MapTile &getTileForEditing(Point<> coord);
    const std::vector<MapTile *> getTilesForEditing(const std::set<int> &indices);
    const MapTile &getTileFromCoord(Point<> coord) const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    Point<> getCoordFromTileIndex(int index);
    int getTileIndexFromCoord(Point<> coord);
    const std::vector<Texture> &getTextures() const;
    std::optional<std::reference_wrapper<const Texture>> getTextureByName(const std::string &name) const;
    bool canSteppedOnTile(Point<> playerCoord);
    bool useOnlyOneMonsterZone() const;
    const std::vector<MonsterZone> &getMonsterZones() const;
    OptMonsterZoneConstRef getMonsterZoneByName(const std::string &zoneName) const;
    bool isShrinkMapImpactAssignedTiles(int offsetLeft,
            int offsetTop,
            int offsetRight,
            int offsetBottom) const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    bool addMonsterZone(const MonsterZone &zone);
    bool replaceMonsterZone(const std::string &name, const MonsterZone &zone);
    bool removeMonsterZone(const std::string &name);
    void resizeMap(int offsetLeft,
                   int offsetTop,
                   int offsetRight,
                   int offsetBottom);
    bool setUseOnlyOneMonsterZone(bool value);
    void unassignMonsterZoneOnAllTiles(int zoneIndex);
    const std::vector<NPC> &getNPCs() const;
    OptNPCConstRef getNPCById(const std::string &id) const;
    bool addNPC(const NPC &npc);
    bool replaceNPC(const std::string &npcId, const NPC &npc);
    bool removeNPC(const std::string &npcId);
    bool addNPCWanderingZone(const std::string &npcId, const std::set<int> &selectedTilesIndices);

 private:
    friend class boost::serialization::access;
    std::string m_lastError;
    std::vector<std::vector<MapTile>> m_tiles;
    std::vector<MonsterZone> m_monsterZones;
    std::vector<NPC> m_npcs;
    TextureContainer m_textureContainer;
    bool m_useOnlyOneMonsterZone;
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
    // Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
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
    }
};

}  // namespace thewarrior::models

BOOST_CLASS_VERSION(thewarrior::models::GameMap, 4)
