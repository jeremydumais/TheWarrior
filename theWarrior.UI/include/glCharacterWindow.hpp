#pragma once

#include "glObjectService.hpp"
#include "glPlayer.hpp"
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "inputDevicesState.hpp"
#include "itemStore.hpp"
#include "point.hpp"
#include <SDL2/SDL_events.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace thewarrior::ui {

class GLCharacterWindow : public GLPopupWindow
{
public:
    GLCharacterWindow();
    virtual ~GLCharacterWindow() = default;
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLPlayer> glPlayer,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore,
                    std::shared_ptr<InputDevicesState> inputDevicesState);
    void update();
    void generateGLElements();
    void render();
    void gameWindowSizeChanged(const Size<> &size);
protected:
    std::shared_ptr<GLPlayer> m_glPlayer;
    std::shared_ptr<ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    GLTexture m_slotsGLTexture;
    void addSlot(Point<float> location);
    void addItemToSlot(const Item *item, Point<float> location);
};

} // namespace thewarrior::ui
