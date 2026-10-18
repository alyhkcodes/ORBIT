#include <iostream>
#include <string>
using namespace std;

struct timestamp {
    int date, month, year;
    int hour, minute;
};


// Use setter/getter to assign values in objects for abstraction
class found_items {
private:
    string item_ID, category, brand, colour, location;
    timestamp ts;
    int score;   // to store matching score

public:
    // Setters
    void setItemID(const string& id) { item_ID = id; }
    void setCategory(const string& c) { category = c; }
    void setBrand(const string& b) { brand = b; }
    void setColour(const string& col) { colour = col; }
    void setLocation(const string& loc) { location = loc; }
    void setTimestamp(const timestamp& t) { ts = t; }
    void setScore(int s) { score = s; }

    // Getters
    string getItemID() const { return item_ID; }
    string getCategory() const { return category; }
    string getBrand() const { return brand; }
    string getColour() const { return colour; }
    string getLocation() const { return location; }
    timestamp getTimestamp() const { return ts; }
    int getScore() const { return score; }
};

// Qodo: Test this class
class lost_items {
private:
    string item_ID, category, brand, colour, location;
    timestamp ts;

public:
    // Setters
    void setItemID(const string& id) { item_ID = id; }
    void setCategory(const string& c) { category = c; }
    void setBrand(const string& b) { brand = b; }
    void setColour(const string& col) { colour = col; }
    void setLocation(const string& loc) { location = loc; }
    void setTimestamp(const timestamp& t) { ts = t; }

    // Getters
    string getItemID() const { return item_ID; }
    string getCategory() const { return category; }
    string getBrand() const { return brand; }
    string getColour() const { return colour; }
    string getLocation() const { return location; }
    timestamp getTimestamp() const { return ts; }
};
