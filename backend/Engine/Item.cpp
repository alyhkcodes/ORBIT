#include <string>
#include <utility>
#include "item.h"

using namespace std;

namespace orbit {

string normalize(const string& value);

Item::Item(string id, string category, int month, string color,
           string brand, string description, string location,
           long long occurredAt, string status)
    : id_(move(id)), category_(move(category)), month_(month),
      color_(move(color)), brand_(move(brand)),
      description_(move(description)), location_(move(location)),
      occurredAt_(occurredAt), status_(move(status)) {}

const string& Item::id() const { return id_; }
const string& Item::category() const { return category_; }
int Item::month() const { return month_; }
const string& Item::color() const { return color_; }
const string& Item::brand() const { return brand_; }
const string& Item::description() const { return description_; }
const string& Item::location() const { return location_; }
long long Item::occurredAt() const { return occurredAt_; }
const string& Item::status() const { return status_; }

string Item::pruningKey() const {
    return normalize(category_) + "|" + to_string(month_);
}

LostItem::LostItem(string id, string category, int month, string color,
                   string brand, string description, string location,
                   long long occurredAt, string status, string reporterContact)
    : Item(move(id), move(category), month, move(color), move(brand),
           move(description), move(location), occurredAt, move(status)),
      reporterContact_(move(reporterContact)) {}

const char* LostItem::kind() const { return "LOST"; }

FoundItem::FoundItem(string id, string category, int month, string color,
                     string brand, string description, string location,
                     long long occurredAt, string status, string finderContact,
                     string currentLocation)
    : Item(move(id), move(category), month, move(color), move(brand),
           move(description), move(location), occurredAt, move(status)),
      finderContact_(move(finderContact)), currentLocation_(move(currentLocation)) {}

const char* FoundItem::kind() const { return "FOUND"; }

}  // namespace orbit
