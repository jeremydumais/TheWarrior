#pragma once


#include "glComponentController.hpp"
namespace mapeditor::controllers {

class TilePropsComponentController {
 public:
    explicit TilePropsComponentController(GLComponentController *glComponentController);
 private:
    GLComponentController *m_glComponentController = nullptr;
};

}  // namespace mapeditor::controllers

