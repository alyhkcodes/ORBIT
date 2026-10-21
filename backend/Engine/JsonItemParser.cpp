#include <cstdio>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include "item.h"

using namespace std;
using namespace nlohmann;

namespace orbit {

string trim(const string& value);

namespace {

string requiredString(const json& object, const char* field) {
    if (!object.contains(field) || !object[field].is_string()) {
        throw invalid_argument(string("'") + field + "' must be a string");
    }
    string value = trim(object[field].get<string>());
    if (value.empty()) throw invalid_argument(string("'") + field + "' cannot be empty");
    return value;
}

string optionalString(const json& object, const char* field) {
    if (!object.contains(field) || object[field].is_null()) return {};
    if (!object[field].is_string()) {
        throw invalid_argument(string("'") + field + "' must be a string or null");
    }
    return trim(object[field].get<string>());
}

bool isLeapYear(int year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

int daysInMonth(int year, int month) {
    static const int lengths[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && isLeapYear(year) ? 29 : lengths[month - 1];
}

long long daysFromCivil(int year, unsigned month, unsigned day) {
    year -= month <= 2;
    const int era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yearOfEra = static_cast<unsigned>(year - era * 400);
    const unsigned adjustedMonth = static_cast<unsigned>(static_cast<int>(month) + (month > 2 ? -3 : 9));
    const unsigned dayOfYear = (153 * adjustedMonth + 2) / 5 + day - 1;
    const unsigned dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
    return static_cast<long long>(era) * 146097 + static_cast<long long>(dayOfEra) - 719468;
}

long long parseOccurredAt(const string& value) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    char separator = '\0';
    const int parsed = sscanf(value.c_str(), "%d-%d-%d%c%d:%d:%d",
                                   &year, &month, &day, &separator, &hour, &minute, &second);
    if (parsed < 3 || year < 1 || month < 1 || month > 12 || day < 1 ||
        day > daysInMonth(year, month)) {
        throw invalid_argument("'occurredAt' must be a valid ISO date or date-time string");
    }

    int offsetMinutes = 0;
    const auto timeMarker = value.find_first_of("T ");
    if (timeMarker != string::npos) {
        if (parsed < 7 || hour > 23 || minute > 59 || second > 59) {
            throw invalid_argument("'occurredAt' must contain a valid time");
        }
        const auto zone = value.find_first_of("Z+-", timeMarker + 1);
        if (zone != string::npos && value[zone] != 'Z') {
            int zoneHour = 0, zoneMinute = 0;
            char sign = value[zone];
            if (sscanf(value.c_str() + zone, "%c%d:%d", &sign, &zoneHour, &zoneMinute) != 3 ||
                zoneHour > 23 || zoneMinute > 59) {
                throw invalid_argument("'occurredAt' has an invalid timezone offset");
            }
            offsetMinutes = (zoneHour * 60 + zoneMinute) * (sign == '+' ? 1 : -1);
        }
    }

    const long long dateSeconds = daysFromCivil(year, static_cast<unsigned>(month),
                                                static_cast<unsigned>(day)) * 86400;
    return dateSeconds + hour * 3600LL + minute * 60LL + second - offsetMinutes * 60LL;
}

}  // namespace

Item* parseItem(const json& value, const string& expectedKind) {
    if (!value.is_object()) throw invalid_argument("each item must be a JSON object");
    const string id = requiredString(value, "id");
    if (requiredString(value, "kind") != expectedKind) {
        throw invalid_argument("item 'kind' must be " + expectedKind);
    }

    const string category = requiredString(value, "category");
    if (!value.contains("month") || !value["month"].is_number_integer()) {
        throw invalid_argument("'month' must be an integer from 1 to 12");
    }
    const int month = value["month"].get<int>();
    if (month < 1 || month > 12) throw invalid_argument("'month' must be an integer from 1 to 12");

    const string color = requiredString(value, "color");
    const string brand = optionalString(value, "brand");
    const string description = requiredString(value, "description");
    const string location = requiredString(value, "location");
    const string status = value.contains("status") ? requiredString(value, "status") : "REPORTED";
    static const unordered_set<string> statuses = {"REPORTED", "MATCH_PENDING", "MATCHED", "RESOLVED"};
    if (statuses.find(status) == statuses.end()) throw invalid_argument("'status' is not a supported item status");
    const long long occurredAt = parseOccurredAt(requiredString(value, "occurredAt"));

    if (expectedKind == "LOST") {
        return new LostItem(id, category, month, color, brand, description,
                            location, occurredAt, status,
                            requiredString(value, "reporterContact"));
    }
    return new FoundItem(id, category, month, color, brand, description,
                         location, occurredAt, status,
                         requiredString(value, "finderContact"),
                         requiredString(value, "currentLocation"));
}

}  // namespace orbit
