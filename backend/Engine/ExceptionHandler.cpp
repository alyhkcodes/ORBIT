#include "ExceptionHandler.h"

#include <exception>

using namespace std;
using namespace nlohmann;

namespace orbit {

json matchRequest(const json& request);

json handleRequestLine(const string& line) {
    try {
        return matchRequest(json::parse(line));
    } catch (const json::parse_error& error) {
        return {
            {"error", {{"code", "INVALID_JSON"}, {"message", error.what()}}}
        };
    } catch (const exception& error) {
        return {
            {"error", {{"code", "INVALID_REQUEST"}, {"message", error.what()}}}
        };
    }
}

}  // namespace orbit
