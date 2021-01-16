#pragma once

#include "gameMap.hpp"
#include "point.hpp"
#include <boost/serialization/access.hpp>
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
    bool isShrinkMapImpactAssignedTiles(int offsetLeft, 
                                        int offsetTop, 
                                        int offsetRight, 
                                        int offsetBottom) const;
    void resizeMap(int offsetLeft, 
                   int offsetTop, 
                   int offsetRight, 
                   int offsetBottom);
    void replaceTilesTextureName(const std::string &oldName, const std::string &newName);
private:
    friend class boost::serialization::access;
    std::string lastError;
	std::shared_ptr<GameMap> map;
};