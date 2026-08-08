#pragma once

#include <qpixmap.h>
#include <memory>
#include <string>
#include <vector>
#include "glComponentController.hpp"
#include "iTexturePixmapProvider.hpp"
#include "npcDTO.hpp"
#include "texture.hpp"
#include "conversationScenarioSummaryDTO.hpp"

namespace mapeditor::controllers {

class EditNPCFormController {
 public:
    struct NPCPixmapResult {
        bool success;
        std::shared_ptr<QPixmap> result;
    };

    EditNPCFormController(const GLComponentController *glComponentController,
                          const std::string &resourcesPath,
                          const std::vector<thewarrior::models::Texture> &textures,
                          commoneditor::ui::ITexturePixmapProvider &pixmapProvider,
                          const std::optional<mapeditor::controllers::NPCDTO> selectedNPC,
                          const std::vector<std::string> &alreadyUsedNPCIds);
    const GLComponentController *getGLComponentController() const;                     
    const std::string &getResourcesPath() const;
    const std::string &getLastError() const;
    bool isEditMode() const;
    const std::vector<thewarrior::models::Texture> &getTextures();
    NPCPixmapResult getNPCPixmap(const std::string &textureName,
                                 const int baseTextureIndex);
    const std::optional<mapeditor::controllers::NPCDTO> &getSelectedNPC() const;                            
    bool isNPCIdAlreadyUsed(const std::string &id) const;
    static std::vector<std::string> convertPlainTextToLines(const QString &text);
    bool isDTOValid(mapeditor::controllers::NPCDTO &dto);
    const std::vector<thewarrior::models::ConversationScenario> &getConversationScenarios() const;
    std::vector<ConversationScenarioSummaryDTO> getConversationScenariosDTO() const;
    std::vector<thewarrior::models::ConversationNodeId> getAlreadyUsedScenarioIds() const;
    std::optional<thewarrior::models::ConversationScenario> getConversationScenarioById(const thewarrior::models::ConversationScenarioId &scenarioId) const;
    void addConversationScenario(const thewarrior::models::ConversationScenario &scenario);
    bool updateConversationScenario(const thewarrior::models::ConversationScenarioId &oldConversationScenarioId,
                                    const thewarrior::models::ConversationScenario &scenario);
    bool removeConversationScenario(const thewarrior::models::ConversationScenarioId &oldConversationScenarioId);

 private:
    const GLComponentController *m_glComponentController = nullptr;
    std::string m_resourcesPath;
    std::string m_lastError;
    const std::vector<thewarrior::models::Texture> &m_textures;
    commoneditor::ui::ITexturePixmapProvider &m_texturePixmapProvider;
    const std::optional<mapeditor::controllers::NPCDTO> m_selectedNPC;
    const std::vector<std::string> m_alreadyUsedNPCIds;
    std::vector<thewarrior::models::ConversationScenario> m_conversationScenarios;
};

}  // namespace mapeditor::controllers
