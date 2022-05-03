#include "textureUtils.hpp"

int TextureUtils::getTextureIndexFromPosition(const Point &pos, const Texture &texture) 
{
    int realY { texture.getHeight() - pos.y()};
	int indexX = pos.x() / texture.getTileWidth();
	int indexY = realY / texture.getTileHeight();
	int tileIndex { indexX + (indexY * (texture.getWidth()/texture.getTileWidth())) };
	return tileIndex;
}

QPixmap TextureUtils::getTextureTileImageFromTexture(const QPixmap *sourcePixmap, int tileIndex, const Texture &texture)
{
	int textureWidthInPixel { texture.getWidth() };
	int textureHeightInPixel { texture.getHeight() };
	int x { (tileIndex * texture.getTileWidth()) % textureWidthInPixel };
	int y { textureHeightInPixel - (((tileIndex * texture.getTileWidth()) / textureWidthInPixel) * texture.getTileHeight()) };
    QPixmap imagePart = sourcePixmap->copy(x, 
										   y - texture.getTileHeight(), 
									       texture.getTileWidth(), 
										   texture.getTileHeight());																				
	return imagePart;
}