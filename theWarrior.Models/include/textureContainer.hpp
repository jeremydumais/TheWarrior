#pragma once

#include "texture.hpp"
#include <boost/optional.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <vector>

class TextureContainer
{
public:
    TextureContainer();
    const std::string &getLastError() const;
    const std::vector<Texture> &getTextures() const;
    size_t getCount() const;
    boost::optional<const Texture &> getTextureByName(const std::string &name) const;
    bool addTexture(const TextureInfo &textureInfo);
    bool replaceTexture(const std::string &name, const TextureInfo &textureInfo);
    bool removeTexture(const std::string &name);
private:
    friend class boost::serialization::access;
    std::string lastError;
    std::vector<Texture> textures;
    std::vector<Texture>::iterator getTextureIterator(const std::string &name);
    //Serialization method
    template<class Archive>
    void serialize(Archive & ar, const unsigned int)
    {
        ar & textures;
    }
};

BOOST_CLASS_VERSION(TextureContainer, 0)