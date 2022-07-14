#pragma once

#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "inputDevicesState.hpp"
#include <boost/signals2.hpp>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_stdinc.h>
#include <memory>
#include <string>
#include <vector>

class GLChoicePopup
{
public:
    GLChoicePopup();
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLFormService> glFormService,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<InputDevicesState> inputDevicesState);
    void preparePopup(std::vector<std::string> choices);
    void update();
    void render();
    void generateGLElements();
    void gameWindowLocationChanged(const Point<float> &windowCenter);
    boost::signals2::signal<void(size_t choice)> m_choiceClicked;
    boost::signals2::signal<void()> m_cancelClicked;
private:
    std::shared_ptr<GLFormService> m_glFormService;
    std::shared_ptr<GLTextService> m_textService;
    std::shared_ptr<InputDevicesState> m_inputDevicesState = nullptr;
    Point<float> m_windowCenter;
    size_t m_menuCursorPosition;
    size_t m_menuItemCount;
    //TODO Code the Optional question
    std::string m_optionalQuestion;
    GLTextureService m_textureService;  
    GLTexture m_popupGLTexture;
    std::vector<GLObject> m_menuObjects;
    std::vector<GLTextObject> m_glTextChoices;
    Uint64 lastMoveUpTicks = 0;
    Uint64 lastMoveDownTicks = 0;
    void moveUpPressed();
    void moveDownPressed();
    void actionButtonPressed();
};