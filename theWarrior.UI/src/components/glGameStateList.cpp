#include <fmt/format.h>
#include <cctype>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include "glGameStateList.hpp"
#include "glComponentBase.hpp"
#include "point.hpp"
#include "size.hpp"

using namespace thewarrior::models;

namespace thewarrior::ui::components {

GLGameStateList::GLGameStateList(Point<float> location)
: GLComponentBase(location, Size<float>(700.0F, 700.0F)),
m_focusPosition(0, 0) {}

void GLGameStateList::initialize(const GLComponentBaseInfo &info) {
    GLComponentBase::initialize(info);
}

void GLGameStateList::onGenerateGLElements() {
}

void GLGameStateList::onRender() {
    m_glFormService->drawText({"This is a test", {1.0F, 1.0F}, 0.6F});
}

void GLGameStateList::onGameWindowSizeChanged(const Size<> &) {
    m_location = {
        (m_screenSize.width() / 2.0F) - (m_size.width() / 2.0F),
        (m_screenSize.height() / 2.0F) - (m_size.height() / 2.0F) };
}

void GLGameStateList::buttonUpPress() {
}

void GLGameStateList::buttonDownPress() {
}

void GLGameStateList::buttonLeftPress() {
}

void GLGameStateList::buttonRightPress() {
}

void GLGameStateList::buttonCancelPress() {
}

void GLGameStateList::buttonActionPress() {
}

}  // namespace thewarrior::ui::components
