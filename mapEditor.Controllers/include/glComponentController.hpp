#pragma once

#include "gameMap.hpp"
#include <memory>
#include <string>
#include <vector>

namespace mapeditor::controllers {

class GLComponentController
{
public:
    GLComponentController();
    const std::shared_ptr<thewarrior::models::GameMap> getMap() const;
    void setCurrentMap(std::shared_ptr<thewarrior::models::GameMap> map);
    std::vector<std::string> getAlreadyUsedTextureNames() const;
    bool isTextureUsedInMap(const std::string &name);
    bool isShrinkMapImpactAssignedTiles(int offsetLeft,
                                        int offsetTop,
                                        int offsetRight,
                                        int offsetBottom) const;
    void resizeMap(int offsetLeft,
                   int offsetTop,
                   int offsetRight,
                   int offsetBottom);
private:
    std::shared_ptr<thewarrior::models::GameMap> m_map;
};

} // namespace mapeditor::controllers
