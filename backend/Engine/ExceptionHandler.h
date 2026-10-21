#pragma once

#include <nlohmann/json.hpp>
#include <string>

using namespace std;

namespace orbit {

nlohmann::json handleRequestLine(const string& line);

}  // namespace orbit
