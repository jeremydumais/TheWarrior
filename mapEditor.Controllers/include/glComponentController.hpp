#pragma once

#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "gameMap.hpp"
#include "mapTile.hpp"
#include "mapTileDTO.hpp"
#include "monsterZone.hpp"
#include "monsterZoneDTO.hpp"
#include "point.hpp"
#include "textureDTO.hpp"

namespace mapeditor::controllers {

typedef std::optional<const mapeditor::controllers::MonsterZoneDTO> OptMonsterZoneDTOConst;

class GLComponentController {
 public:
    GLComponentController();
    const std::shared_ptr<thewarrior::models::GameMap> getMap() const;
    const std::string &getLastError() const;
    std::vector<thewarrior::models::MapTile *> getCurrentMapTiles();
    std::vector<MapTileDTO> getSelectedMapTiles() const;
    std::vector<std::string> getAlreadyUsedTextureNames() const;
    std::vector<std::string> getAlreadyUsedMonsterZoneNames() const;
    bool isUseOnlyOneMonsterZone() const;
    bool isTextureUsedInMap(const std::string &name);
    bool isShrinkMapImpactAssignedTiles(int offsetLeft,
                                        int offsetTop,
                                        int offsetRight,
                                        int offsetBottom) const;
    std::vector<mapeditor::controllers::MonsterZoneDTO> getMonsterZones() const;
    OptMonsterZoneDTOConst getMonsterZoneByName(const std::string &name) const;
    boost::optional<thewarrior::models::Point<int>> getCoordFromSingleSelectedTile() const;
    size_t getHistoryCurrentIndex() const;
    size_t getHistoryCount() const;
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    void selectTilesForEditing(const std::set<int> &indices);
    void resizeMap(int offsetLeft,
            int offsetTop,
            int offsetRight,
            int offsetBottom);
    void setLastSelectedTexture(const std::string &name, int index);
    void setLastSelectedObject(const std::string &name, int index);
    void clearLastSelectedTexture();
    void clearLastSelectedObject();
    void setLastSelectedMonsterZone(int index);
    void clearLastSelectedMonsterZone();
    void unselectMapTiles();
    void clearEditHistory();
    void undo();
    void redo();
    void pushCurrentStateToHistory();
    void applyTexture();
    void applyObject();
    void applyCanStep(bool value);
    bool applyDenyZone(const std::string &eventStr);
    void clearDenyZones();
    void applyMonsterZone();
    void clearMonsterZone();
    bool addTexture(const commoneditor::ui::TextureDTO &textureDTO);
    bool replaceTexture(const std::string &name, const commoneditor::ui::TextureDTO &textureDTO);
    bool removeTexture(const std::string &name);
    bool addMonsterZone(const MonsterZoneDTO &monsterZoneDTO);
    bool replaceMonsterZone(const std::string &name, const MonsterZoneDTO &monsterZoneDTO);
    bool removeMonsterZone(const std::string &name);
    bool setUseOnlyOneMonsterZone(bool value);

 private:
    std::shared_ptr<thewarrior::models::GameMap> m_map;
    std::vector<thewarrior::models::MapTile *> m_currentMapTiles;
    std::set<int> m_selectedIndices;
    std::vector<std::shared_ptr<thewarrior::models::GameMap>> m_editHistory;
    size_t m_historyCurrentIndex = 0;
    std::string m_lastError;
    std::string m_lastSelectedTextureName;
    std::string m_lastSelectedObjectName;
    int m_lastSelectedTextureIndex;
    int m_lastSelectedObjectIndex;
    int m_lastSelectedMonsterZoneIndex;
    void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
};

}  // namespace mapeditor::controllers
