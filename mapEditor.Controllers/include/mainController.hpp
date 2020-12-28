#pragma once

#include "gameMap.hpp"
#include "point.hpp"
#include <memory>
#include <string>
#include <vector>

class MainController
{
public:
    MainController();
    const std::string &getLastError() const;
    std::shared_ptr<GameMap> getMap();
    std::vector<std::string> getAlreadyUsedTextureNames() const;
    int getTextureIndexFromPosition(const Point &pos, const Texture &texture);
    bool createMap(unsigned int width, unsigned int height);
    const std::vector<Texture> &getTextures() const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
    bool isTextureUsedInMap(const std::string &name);
    void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
private:
    std::string lastError;
	std::shared_ptr<GameMap> map;
};