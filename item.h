#ifndef ITEM_H
#define ITEM_H

#include <string>

struct Item {
    int id = 0;
    std::string type;        // "LOST" or "FOUND"
    std::string title;
    std::string category;
    std::string description;
    std::string location;
    std::string timestamp;
    bool resolved = false;
};

#endif
