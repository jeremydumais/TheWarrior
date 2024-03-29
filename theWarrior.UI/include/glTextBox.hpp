#pragma once

#include "glFormService.hpp"
#include "glObjectService.hpp"
#include "glPopupWindow.hpp"
#include "glShaderProgram.hpp"
#include "glTextService.hpp"
#include "glTexture.hpp"
#include "glTextureService.hpp"
#include "itemStore.hpp"
#include "IShaderService.hpp"
#include "message.hpp"
#include "messageDTO.hpp"
#include "point.hpp"
#include "size.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <string>
#include <vector>

namespace thewarrior::ui {

class GLTextBox : public GLPopupWindow
{
public:
    GLTextBox();
    ~GLTextBox() = default;
    void initialize(const std::string &resourcePath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<thewarrior::models::ItemStore> itemStore,
            const std::map<std::string, unsigned int> *texturesGLItemStore);
    const std::string &getLastError() const;
    void generateMessage(std::shared_ptr<thewarrior::ui::controllers::MessageDTO> messageDTO);
    void draw();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);
private:
    thewarrior::models::Size<float> m_screenSize;
    GLTextureService m_textureService;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    std::string m_lastError;
    std::shared_ptr<thewarrior::ui::controllers::MessageDTO> m_messageDTO;
    ComputedTextForDisplay m_computedTextForDisplay;
    const float BOXPADDING = 60.0F;
    const float ITEMICONSIZE = 60.0F;
    float getImageHeight() const;
};

} // namespace thewarrior::ui
