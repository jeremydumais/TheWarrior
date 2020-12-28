#pragma once

#include "gameMap.hpp"
#include "point.hpp"
#include <memory>
#include <vector>

class MainController
{
public:
    MainController();
    void createMap(unsigned int width, unsigned int height);
    std::shared_ptr<GameMap> getMap();
    std::vector<std::string> getAlreadyUsedTextureNames() const;
    int getTextureIndexFromPosition(const Point &pos, const Texture &texture);
    void generateTestMap();
    void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
private:
	std::shared_ptr<GameMap> map;
};