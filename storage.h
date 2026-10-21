#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <vector>
#include "item.h"

class Storage {
private:
    std::string filename;

public:
    explicit Storage(const std::string& filename = "orbit_data.txt");

    std::vector<Item> loadItems() const;
    bool saveItems(const std::vector<Item>& items) const;
    int getNextId(const std::vector<Item>& items) const;
};

#endif
