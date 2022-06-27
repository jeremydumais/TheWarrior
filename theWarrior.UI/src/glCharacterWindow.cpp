#include "glCharacterWindow.hpp"

const float SLOTSIZE = 80.0F;

GLCharacterWindow::GLCharacterWindow()
    : GLPopupWindow({ 800.0F, 600.0F }),
      m_slotsGLTexture({ Texture(TextureInfo{ "emptySlot", "item_slot.png", 768, 256, 256, 256 }), 0 })
{
}

void GLCharacterWindow::initialize(const std::string &resourcePath,
                                   std::shared_ptr<GLPlayer> glPlayer,
                                   std::shared_ptr<GLTextService> textService,
                                   std::shared_ptr<ItemStore> itemStore,
                                   const std::map<std::string, unsigned int> *texturesGLItemStore)
{
    GLPopupWindow::initialize("Character", resourcePath, textService);
    m_glPlayer = glPlayer;
    m_itemStore = itemStore;
    m_texturesGLItemStore = texturesGLItemStore;
    m_textureService.loadTexture(m_slotsGLTexture);
}

void GLCharacterWindow::processEvents(SDL_Event &e)
{
}

void GLCharacterWindow::generateGLElements()
{
    GLPopupWindow::generateGLElements();
    //Equipments
    auto equipment = m_glPlayer->getEquipment();
    m_glObjects.resize(39);
    generateBoxQuad(m_glObjects.begin(), {20.0F, 80.0F}, {380.0F, 500.0F}, &m_windowGLTexture.texture, 0, m_windowGLTexture.glTextureId);
    generateBoxQuad(m_glObjects.begin()+8, {420.0F, 80.0F}, {360.0F, 500.0F}, &m_windowGLTexture.texture, 0, m_windowGLTexture.glTextureId);
    generateBoxQuad(m_glObjects.begin()+16, {125.0F, 93.0F}, {167.0F, 40.0F}, &m_windowGLTexture.texture, 17, m_windowGLTexture.glTextureId);
    generateBoxQuad(m_glObjects.begin()+24, {488.0F, 93.0F}, {222.0F, 40.0F}, &m_windowGLTexture.texture, 17, m_windowGLTexture.glTextureId);
    generateQuad(*(m_glObjects.begin()+32), {55.0F, 190.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Main hand
    generateQuad(*(m_glObjects.begin()+33), {170.0F, 150.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Head
    generateQuad(*(m_glObjects.begin()+34), {285.0F, 190.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Second hand
    generateQuad(*(m_glObjects.begin()+35), {285.0F, 285.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Hands
    generateQuad(*(m_glObjects.begin()+36), {170.0F, 270.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Upper body
    generateQuad(*(m_glObjects.begin()+37), {170.0F, 365.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Lower body
    generateQuad(*(m_glObjects.begin()+38), {170.0F, 460.0F}, {SLOTSIZE, SLOTSIZE}, &m_slotsGLTexture.texture, 0, m_slotsGLTexture.glTextureId); //Feet
    addXCenteredTextObject({"Equipment", {0.0F, 120.0F}, 0.5F}, 20.0F, 380.0F);
    addXCenteredTextObject({"Infos and Stats", {0.0F, 120.0F}, 0.5F}, 420.0F, 360.0F);
    if (!equipment.getMainHand().has_value()) {
        addXCenteredTextObject({"Main", {0.0F, 225.0F}, 0.3F}, 55.0F, 80.0F);
        addXCenteredTextObject({"hand", {0.0F, 245.0F}, 0.3F}, 55.0F, 80.0F);
    }
    else {
        
    }
    addXCenteredTextObject({"Head", {0.0F, 195.0F}, 0.3F}, 170.0F, 80.0F);
    addXCenteredTextObject({"Second.", {0.0F, 225.0F}, 0.3F}, 285.0F, 80.0F);
    addXCenteredTextObject({"hand", {0.0F, 245.0F}, 0.3F}, 285.0F, 80.0F);
    addXCenteredTextObject({"Hands", {0.0F, 330.0F}, 0.3F}, 285.0F, 80.0F);
    addXCenteredTextObject({"Upper", {0.0F, 305.0F}, 0.3F}, 170.0F, 80.0F);
    addXCenteredTextObject({"body", {0.0F, 325.0F}, 0.3F}, 170.0F, 80.0F);
    addXCenteredTextObject({"Lower", {0.0F, 400.0F}, 0.3F}, 170.0F, 80.0F);
    addXCenteredTextObject({"body", {0.0F, 420.0F}, 0.3F}, 170.0F, 80.0F);
    addXCenteredTextObject({"Feet", {0.0F, 505.0F}, 0.3F}, 170.0F, 80.0F);
}
    
void GLCharacterWindow::render()
{
    GLPopupWindow::render();

}

void GLCharacterWindow::gameWindowSizeChanged(const Size<> &size)
{
    GLPopupWindow::gameWindowSizeChanged(size);
    generateGLElements();
}