#pragma once

#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include "editorStateInfo.hpp"

namespace mapeditor::controllers {

class EditorStateInfoJSONSerializer {
 public:
    static boost::property_tree::ptree serialize(const std::vector<EditorStateInfo> &editorStates);
    static std::vector<EditorStateInfo> deserialize(boost::property_tree::ptree editorStateNode);
};

}  // namespace mapeditor::controllers

