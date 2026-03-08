#include <memory>
#include <utility>
#include "glNPC.hpp"
#include "direction.hpp"
#include "npc.hpp"
#include "texture.hpp"

using thewarrior::models::Direction;
using thewarrior::models::NPC;
using thewarrior::models::Texture;

namespace thewarrior::ui {

GLNPC::GLNPC(const NPC &npc, const thewarrior::models::Texture &texture)
    : NPC(npc), m_texture(texture), m_direction(Direction::None) {}

void GLNPC::initialize(const TileSize &tileSize,
                       std::shared_ptr<thewarrior::models::WorldState> worldState) {
  m_tileSize = tileSize;
  m_worldState = std::move(worldState);
  m_direction = Direction::None;
  m_worldState->setNPCPosition(getId(), getSpawnPosition());
  setFacing(getDefaultFacing());
  generateGLObject();
  setGLObjectPosition();
}

void GLNPC::draw() const {
  glBindTexture(GL_TEXTURE_2D, glTextureId);
  glBindVertexArray(glObject.vao);
  glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, glObject.vboColor);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, glObject.vboTexture);
  glEnableVertexAttribArray(2);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);
}

void GLNPC::generateGLObject() {
  GLfloat texColorBuf[4][3]{{1.0F, 1.0F, 1.0F}, /* Red */
                            {1.0F, 1.0F, 1.0F}, /* Green */
                            {1.0F, 1.0F, 1.0F}, /* Blue */
                            {1.0F, 1.0F, 1.0F}};
  auto tileHalfWidth = m_tileSize.tileHalfWidth;
  auto tileHalfHeight = m_tileSize.tileHalfHeight;

  float startPosX{-1.0F + tileHalfWidth};
  float startPosY{1.0F - tileHalfHeight};

  GLfloat tileCoord[4][2] = {
      {-tileHalfWidth + startPosX,
       tileHalfHeight + startPosY}, /* Top Left point */
      {tileHalfWidth + startPosX,
       tileHalfHeight + startPosY}, /* Top Right point */
      {tileHalfWidth + startPosX,
       -tileHalfHeight + startPosY}, /* Bottom Right point */
      {-tileHalfWidth + startPosX,
       -tileHalfHeight + startPosY}}; /* Bottom Left point */

  GenerateGLObjectInfo infoGenTexture{.glObject = &glObject,
                                      .texture = &m_texture,
                                      .textureIndex = getCurrentFacingTextureIndex()};
  GLObjectService::generateGLObject(infoGenTexture, tileCoord, texColorBuf);
}

void GLNPC::unloadGLObject() {
  glDeleteBuffers(1, &glObject.vboPosition);
  glDeleteBuffers(1, &glObject.vboColor);
  glDeleteBuffers(1, &glObject.vboTexture);
  glDeleteVertexArrays(1, &glObject.vao);
}

void GLNPC::setGLTextureId(unsigned int textureId) { glTextureId = textureId; }

void GLNPC::setGLObjectPosition() const {
  glBindVertexArray(glObject.vao);

  auto tileWidth = m_tileSize.tileWidth;
  auto tileHeight = m_tileSize.tileHalfHeight * 2.0F;
  auto tileHalfWidth = m_tileSize.tileHalfWidth;
  auto tileHalfHeight = m_tileSize.tileHalfHeight;
  // Set Tile Coord To Origin
  GLfloat m_tileCoordBuf[4][2];
  float startPosX{-1.0F + tileHalfWidth};
  float startPosY{1.0F - tileHalfHeight};

  m_tileCoordBuf[0][0] = -tileHalfWidth + startPosX + tileWidth;
  m_tileCoordBuf[0][1] = tileHalfHeight + startPosY - tileHalfHeight;
  m_tileCoordBuf[1][0] = tileHalfWidth + startPosX + tileWidth;
  m_tileCoordBuf[1][1] = tileHalfHeight + startPosY - tileHalfHeight;
  m_tileCoordBuf[2][0] = tileHalfWidth + startPosX + tileWidth;
  m_tileCoordBuf[2][1] = -tileHalfHeight + startPosY - tileHalfHeight;
  m_tileCoordBuf[3][0] = -tileHalfWidth + startPosX + tileWidth;
  m_tileCoordBuf[3][1] = -tileHalfHeight + startPosY - tileHalfHeight;

  auto npcPosition = getPosition();
  for (int i = 0; i < 4; i++) {
      m_tileCoordBuf[i][0] += (static_cast<float>(npcPosition.x()) - 1.0F) * tileWidth;
      m_tileCoordBuf[i][1] -= (static_cast<float>(npcPosition.y()) * tileHeight) - tileHalfHeight;
  }
  glBindBuffer(GL_ARRAY_BUFFER, glObject.vboPosition);
  glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), m_tileCoordBuf,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);
}

void GLNPC::onGameWindowTileSizeChanged(const TileSize &tileSize) {
  m_tileSize = tileSize;
  unloadGLObject();
  generateGLObject();
  setGLObjectPosition();
}

const thewarrior::models::Point<size_t> &GLNPC::getPosition() const {
    return m_worldState->getNPCPosition(getId());
}

thewarrior::models::NPCFacing GLNPC::getFacing() {
    return getCurrentFacing();
}

void GLNPC::setFacing(thewarrior::models::NPCFacing facing) {
  setCurrentFacing(facing);
  m_worldState->setNPCFacing(getId(), facing);
}

}  // namespace thewarrior::ui
