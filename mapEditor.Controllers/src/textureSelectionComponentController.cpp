#include "textureSelectionComponentController.hpp"

int TextureSelectionComponentController::getTextureIndexFromPosition(const Point &pos, const Texture &texture) 
{
    int realY { texture.getHeight() - pos.y()};
	int indexX = pos.x() / texture.getTileWidth();
	int indexY = realY / texture.getTileHeight();
	int tileIndex { indexX + (indexY * (texture.getWidth()/texture.getTileWidth())) };
	return tileIndex;
}
