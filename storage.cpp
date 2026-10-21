#include "storage.h"

#include <fstream>
#include <iomanip>
#include <algorithm>

Storage::Storage(const std::string& filename) : filename(filename) {}

std::vector<Item> Storage::loadItems() const {
    std::vector<Item> items;
    std::ifstream file(filename);

    if (!file.is_open()) {
        return items; // First run: the data file does not exist yet.
    }

    Item item;
    int resolvedValue;

    while (file >> item.id
                >> std::quoted(item.type)
                >> std::quoted(item.title)
                >> std::quoted(item.category)
                >> std::quoted(item.description)
                >> std::quoted(item.location)
                >> std::quoted(item.timestamp)
                >> resolvedValue) {
        item.resolved = (resolvedValue != 0);
        items.push_back(item);
    }

    return items;
}

bool Storage::saveItems(const std::vector<Item>& items) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    for (const Item& item : items) {
        file << item.id << ' '
             << std::quoted(item.type) << ' '
             << std::quoted(item.title) << ' '
             << std::quoted(item.category) << ' '
             << std::quoted(item.description) << ' '
             << std::quoted(item.location) << ' '
             << std::quoted(item.timestamp) << ' '
             << (item.resolved ? 1 : 0) << '\n';
    }

    return true;
}

int Storage::getNextId(const std::vector<Item>& items) const {
    int highestId = 0;

    for (const Item& item : items) {
        if (item.id > highestId) {
            highestId = item.id;
        }
    }

    return highestId + 1;
}
