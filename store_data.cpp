#include <iostream>
#include <vector>
#include <string>
#include "items_class.cpp"
#include "json.hpp"  

using namespace std;
using json = nlohmann::json; // to recieve data from node


void storing_data(const json& data) {
    vector<found_items> fi;

    for (const auto& item : data) {
        found_items f;

        f.setItemID(item["item_ID"]);
        f.setCategory(item["category"]);
        f.setBrand(item["brand"]);
        f.setColour(item["colour"]);
        f.setLocation(item["location"]);

        timestamp t;
        t.year   = item["timestamp"]["year"];
        t.month  = item["timestamp"]["month"];
        t.date   = item["timestamp"]["date"];
        t.hour   = item["timestamp"]["hour"];
        t.minute = item["timestamp"]["minute"];

        f.setTimestamp(t);
        fi.push_back(f);
    }

    // Now fi contains all parsed items
}


// Here, we might not use const for storing lost-items as once an item was found we have to remove it from the database 

