#pragma once

#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include "toolbarStateInfo.hpp"

namespace mapeditor::controllers {

class ToolBarStateInfoJSONSerializer {
 public:
    static boost::property_tree::ptree serialize(const std::vector<ToolBarStateInfo> &toolbarStates);
    static std::vector<ToolBarStateInfo> deserialize(boost::property_tree::ptree toolbarStateNode);
};

}  // namespace mapeditor::controllers

