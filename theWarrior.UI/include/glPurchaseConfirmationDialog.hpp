#pragma once

#include <map>
#include <memory>
#include <boost/signals2.hpp>
#include "glPopupWindow.hpp"
#include "inputDevicesState.hpp"
#include "itemStore.hpp"

namespace thewarrior::ui {

class GLPurchaseConfirmationDialog : public GLPopupWindow {
 public:
    GLPurchaseConfirmationDialog();
    ~GLPurchaseConfirmationDialog() override = default;
    void initialize(const std::string &resourcePath,
                    std::shared_ptr<GLTextService> textService,
                    std::shared_ptr<thewarrior::models::ItemStore> itemStore,
                    const std::map<std::string, unsigned int> *texturesGLItemStore,
                    std::shared_ptr<InputDevicesState> inputDevicesState,
                    const GLTexture *shopIconsTexture);
    void show(std::shared_ptr<const thewarrior::models::Item> item, unsigned int price);
    void update();
    void generateGLElements();
    void gameWindowSizeChanged(const thewarrior::models::Size<> &size);

    boost::signals2::signal<void()> confirmed;
    boost::signals2::signal<void()> canceled;

 private:
    std::shared_ptr<const thewarrior::models::Item> m_item;
    unsigned int m_price = 0;
    bool m_yesSelected = true;
    std::shared_ptr<thewarrior::models::ItemStore> m_itemStore;
    const std::map<std::string, unsigned int> *m_texturesGLItemStore = nullptr;
    std::shared_ptr<InputDevicesState> m_inputDevicesState;
    const GLTexture *m_shopIconsTexture = nullptr;
};

}  // namespace thewarrior::ui
