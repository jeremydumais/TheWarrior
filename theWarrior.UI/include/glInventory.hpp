#pragma once

#include "glShaderProgram.hpp"
#include "inventory.hpp"
#include "itemStore.hpp"
#include "IShaderService.hpp"
#include "glObjectService.hpp"
#include "glTextService.hpp"
#include "glTextureService.hpp"
#include "point.hpp"
#include "size.hpp"
#include "texture.hpp"
#include <GL/glew.h>
#include <array>
#include <map>
#include <memory>
#include <string>

struct GLTextObject
{
    std::string text;
    Point<float> position;
    float scale;
};

class GLInventory : public IShaderService
{
public:
    GLInventory();
    ~GLInventory() = default;
    bool initShader(const std::string &vertexShaderFileName,
                    const std::string &fragmentShaderFileName) override;
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore);
    const std::string &getLastError() const;
    void setInventory(std::shared_ptr<Inventory> inventory);
    void generateGLInventory();
    void generateQuad(GLObject &object, Point<float> location, Size<float> size, const Texture *texture = nullptr, int textureId = 0);
    void render();
    void gameWindowSizeChanged(const Size<> &size);
private:
    std::shared_ptr<Inventory> m_inventory;
    Point<float> m_inventoryWindowLocation;
    Size<float> m_gameWindowSize;
    std::unique_ptr<GLShaderProgram> m_shaderProgram;
    std::string m_lastError;
    std::shared_ptr<GLTextService> m_textService;
    GLTextureService m_textureService;  
    std::shared_ptr<ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    GLObject m_glInventoryWindow;
    const Size<float> m_inventorySize;
    GLTextObject m_glTitle;
    std::array<GLObject, INVENTORY_MAX> m_slots;
    Texture m_slotsTexture;
    unsigned int m_emptySlotGLTextureId;
    GLObject itemTest;
    void drawQuad(const GLObject &glObject, GLuint textureGLIndex, float transparency = 0.0F);
    void drawText(const GLTextObject &glTextObject);
};