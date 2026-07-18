#pragma once

#include <GL/glew.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
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
    ~GLTextBox() override;
    void initialize(const std::string &resourcePath,
            std::shared_ptr<GLTextService> textService,
            std::shared_ptr<thewarrior::models::ItemStore> itemStore,
            const std::map<std::string, unsigned int> *texturesGLItemStore);
    void generateMessage(std::shared_ptr<thewarrior::ui::controllers::MessageDTO> messageDTO);
    void update(float deltaTime);
    void draw();
    bool isRevealingText() const;
    void revealAllText();
    void clearMessage();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);

 private:
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore;
    std::shared_ptr<thewarrior::ui::controllers::MessageDTO> m_messageDTO;
    ComputedTextForDisplay m_computedTextForDisplay;
    float m_visibleCharacterCount = 0.0F;
    float m_nextPageArrowBlinkElapsedTime = 0.0F;
    bool m_isRevealingText = false;
    bool m_isNextPageArrowGenerated = false;
    std::vector<GLObject> m_nextPageArrowObjects;
    const float BOXPADDING = 60.0F;
    const float ITEMICONSIZE = 60.0F;
    float getImageHeight() const;
    size_t getTotalCharacterCount() const;
    void generateNextPageArrowIfNeeded();
};

}  // namespace thewarrior::ui
