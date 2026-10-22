#include "item.h"
#include "storage.h"
#include "matching.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <string>
#include <vector>

using namespace std;

string readLine(const string& prompt) {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void displayItem(const Item& item) {
    cout << "\n----------------------------------------\n";
    cout << "ID: " << item.id << '\n';
    cout << "Type: " << item.type << '\n';
    cout << "Title: " << item.title << '\n';
    cout << "Category: " << item.category << '\n';
    cout << "Description: " << item.description << '\n';
    cout << "Location: " << item.location << '\n';
    cout << "Date/time: " << item.timestamp << '\n';
    cout << "Status: " << (item.resolved ? "Resolved" : "Active") << '\n';
}

void addItem(vector<Item>& items, const Storage& storage, const string& type) {
    Item item;
    item.id = storage.getNextId(items);
    item.type = type;
    item.title = readLine("Enter item title/name: ");
    item.category = readLine("Enter category (e.g. electronics, wallet, books): ");
    item.description = readLine("Enter description/identifying details: ");
    item.location = readLine("Enter location: ");
    item.timestamp = readLine("Enter date and time (e.g. 20-10-2026, 12:30): ");
    item.resolved = false;

    items.push_back(item);

    if (storage.saveItems(items)) {
        cout << "\n" << type << " item saved successfully. Your item ID is "
             << item.id << ".\n";
    } else {
        cout << "\nError: could not save the data file.\n";
    }
}

void showAllItems(const vector<Item>& items) {
    if (items.empty()) {
        cout << "\nNo items have been added yet.\n";
        return;
    }

    for (const Item& item : items) {
        displayItem(item);
    }
}

void showMatches(const vector<Item>& items) {
    int lostId;
    cout << "Enter the ID of the LOST item: ";

    if (!(cin >> lostId)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    const Item* lostItem = nullptr;

    for (const Item& item : items) {
        if (item.id == lostId && item.type == "LOST") {
            lostItem = &item;
            break;
        }
    }

    if (lostItem == nullptr) {
        cout << "No LOST item found with that ID.\n";
        return;
    }

    vector<MatchResult> matches = findMatches(*lostItem, items, 10);

    if (matches.empty()) {
        cout << "\nNo matching FOUND items yet. Try adding more details or check again later.\n";
        return;
    }

    cout << "\nBest matching FOUND items (highest BM25 score first):\n";
    cout << fixed << setprecision(3);

    for (size_t i = 0; i < matches.size(); i++) {
        const Item* foundItem = nullptr;

        for (const Item& item : items) {
            if (item.id == matches[i].itemId) {
                foundItem = &item;
                break;
            }
        }

        if (foundItem != nullptr) {
            cout << "\nRank #" << (i + 1)
                 << " | BM25 score: " << matches[i].score << '\n';
            displayItem(*foundItem);
        }
    }
}

void markResolved(vector<Item>& items, const Storage& storage) {
    int id;
    cout << "Enter item ID to mark as resolved: ";

    if (!(cin >> id)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid ID.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (Item& item : items) {
        if (item.id == id) {
            item.resolved = true;

            if (storage.saveItems(items)) {
                cout << "Item " << id << " marked as resolved.\n";
            } else {
                cout << "Error: could not save the data file.\n";
            }
            return;
        }
    }

    cout << "No item found with that ID.\n";
}

int main() {
    Storage storage;
    vector<Item> items = storage.loadItems();

    int choice;

    do {
        cout << "\n========== ORBIT: LOST & FOUND ==========\n";
        cout << "1. Report a lost item\n";
        cout << "2. Report a found item\n";
        cout << "3. View all items\n";
        cout << "4. Find matches for a lost item (BM25 + priority queue)\n";
        cout << "5. Mark an item as resolved\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number from the menu.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                addItem(items, storage, "LOST");
                break;
            case 2:
                addItem(items, storage, "FOUND");
                break;
            case 3:
                showAllItems(items);
                break;
            case 4:
                showMatches(items);
                break;
            case 5:
                markResolved(items, storage);
                break;
            case 0:
                cout << "Exiting ORBIT. Your data is saved in orbit_data.txt.\n";
                break;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
