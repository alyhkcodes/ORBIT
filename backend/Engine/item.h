#pragma once

#include <string>

using namespace std;

namespace orbit {

class Item {
public:
    Item(string id, string category, int month, string color,
         string brand, string description, string location,
         long long occurredAt, string status);
    virtual ~Item() = default;
    Item(const Item&) = delete;
    Item& operator=(const Item&) = delete;

    const string& id() const;
    const string& category() const;
    int month() const;
    const string& color() const;
    const string& brand() const;
    const string& description() const;
    const string& location() const;
    long long occurredAt() const;
    const string& status() const;
    string pruningKey() const;
    virtual const char* kind() const = 0;

private:
    string id_;
    string category_;
    int month_;
    string color_;
    string brand_;
    string description_;
    string location_;
    long long occurredAt_;
    string status_;
};

class LostItem final : public Item {
public:
    LostItem(string id, string category, int month, string color,
             string brand, string description, string location,
             long long occurredAt, string status, string reporterContact);
    const char* kind() const override;

private:
    string reporterContact_;
};

class FoundItem final : public Item {
public:
    FoundItem(string id, string category, int month, string color,
              string brand, string description, string location,
              long long occurredAt, string status, string finderContact,
              string currentLocation);
    const char* kind() const override;

private:
    string finderContact_;
    string currentLocation_;
};

}  // namespace orbit
