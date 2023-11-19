#pragma once

#include <memory>
#include <set>
#include <string>
#include <vector>
#include "gameMap.hpp"
#include "mapTile.hpp"
#include "mapTileDTO.hpp"
#include "monsterZone.hpp"
#include "monsterZoneDTO.hpp"

namespace mapeditor::controllers {

typedef std::optional<const mapeditor::controllers::MonsterZoneDTO> OptMonsterZoneDTOConst;

class GLComponentController {
 public:
    GLComponentController();
    const std::shared_ptr<thewarrior::models::GameMap> getMap() const;
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    void selectTilesForEditing(const std::set<int> &indices);
    std::vector<thewarrior::models::MapTile *> getCurrentMapTiles();
    std::vector<MapTileDTO> getSelectedMapTiles() const;
    std::vector<std::string> getAlreadyUsedTextureNames() const;
    std::vector<std::string> getAlreadyUsedMonsterZoneNames() const;
    bool isTextureUsedInMap(const std::string &name);
    bool isShrinkMapImpactAssignedTiles(int offsetLeft,
                                        int offsetTop,
                                        int offsetRight,
                                        int offsetBottom) const;
    void resizeMap(int offsetLeft,
                   int offsetTop,
                   int offsetRight,
                   int offsetBottom);
    std::vector<mapeditor::controllers::MonsterZoneDTO> getMonsterZones() const;
    OptMonsterZoneDTOConst getMonsterZoneByName(const std::string &name) const;
    void unselectMapTiles();

 private:
    std::shared_ptr<thewarrior::models::GameMap> m_map;
    std::vector<thewarrior::models::MapTile *> m_currentMapTiles;
};

}  // namespace mapeditor::controllers
