#pragma once

#include "hugin/export.hpp"
#include <memory>

namespace munin {

class component;

}

namespace hugin {

HUGIN_EXPORT
std::shared_ptr<munin::component> make_background_element();

}

