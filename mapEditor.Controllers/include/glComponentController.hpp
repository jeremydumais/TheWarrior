#pragma once

#include <memory>
#include <string>
#include <vector>
#include "gameMap.hpp"
#include "monsterZoneDTO.hpp"

namespace mapeditor::controllers {

typedef std::optional<const mapeditor::controllers::MonsterZoneDTO> OptMonsterZoneDTOConst;

class GLComponentController {
 public:
    GLComponentController();
    const std::shared_ptr<thewarrior::models::GameMap> getMap() const;
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
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
    bool setUseOnlyOneMonsterZone(bool value);
    bool isUseOnlyOneMonsterZone() const;

 private:
    std::shared_ptr<thewarrior::models::GameMap> m_map;
};

}  // namespace mapeditor::controllers
