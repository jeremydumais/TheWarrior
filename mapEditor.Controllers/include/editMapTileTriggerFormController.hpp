#pragma once

#include <map>
#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include "mapTileTriggerDTO.hpp"

namespace mapeditor::controllers {

class EditMapTileTriggerFormController {
 public:
    EditMapTileTriggerFormController(const boost::optional<mapeditor::controllers::MapTileTriggerDTO> currentTrigger,
            const std::vector<MapTileTriggerDTO> &allTriggers);
    const boost::optional<mapeditor::controllers::MapTileTriggerDTO> &getCurrentTrigger() const;
    int getTriggerEventIndex() const;
    int getTriggerConditionIndex() const;
    int getTriggerActionIndex() const;
    const mapeditor::controllers::MapTileTriggerDTO &getUpdatedTrigger() const;
    const std::string &getLastError() const;
    bool isUpdatedTriggerValid();
    void setEvent(const std::string &event);
    void setCondition(const std::string &condition);
    void setAction(const std::string &action);
    void setActionProperties(const std::map<std::string, std::string> &props);

 private:
    const boost::optional<mapeditor::controllers::MapTileTriggerDTO> m_currentTrigger;
    const std::vector<MapTileTriggerDTO> m_allTriggers;
    mapeditor::controllers::MapTileTriggerDTO m_updatedTrigger;
    std::string m_lastError;
};

}  // namespace mapeditor::controllers

