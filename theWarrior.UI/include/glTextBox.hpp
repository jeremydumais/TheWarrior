#pragma once

#include <GL/glew.h>
#include <map>
#include <memory>
#include <string>
#include "glPopupWindow.hpp"
#include "glTextService.hpp"
#include "itemStore.hpp"
#include "messageDTO.hpp"
#include "size.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace thewarrior::ui {

class GLTextBox : public GLPopupWindow {
 public:
    GLTextBox();
    ~GLTextBox() override = default;
    void initialize(const std::string &resourcePath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<thewarrior::models::ItemStore> itemStore,
            const std::map<std::string, unsigned int> *texturesGLItemStore);
    void generateMessage(std::shared_ptr<thewarrior::ui::controllers::MessageDTO> messageDTO);
    void draw();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);

 private:
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    std::shared_ptr<thewarrior::ui::controllers::MessageDTO> m_messageDTO;
    ComputedTextForDisplay m_computedTextForDisplay;
    const float BOXPADDING = 60.0F;
    const float ITEMICONSIZE = 60.0F;
    float getImageHeight() const;
};

}  // namespace thewarrior::ui
