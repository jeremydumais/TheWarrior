#pragma once

#include "gameMap.hpp"
#include <memory>
#include <string>
#include <vector>

class GLComponentController
{
public:
    GLComponentController();
    const std::shared_ptr<GameMap> getMap() const;
    void setCurrentMap(std::shared_ptr<GameMap> map);
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
    std::shared_ptr<GameMap> m_map;
};

