#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Item {
private:
    string id, category, month, status, kind, color, brand, description, currentLocation, occurredAt;

protected:
    void displayCommonDetails() const {
        cout << "ID: " << id
             << "\nKind: " << kind
             << "\nCategory: " << category
             << "\nMonth: " << month
             << "\nStatus: " << status
             << "\nColor: " << color
             << "\nBrand: " << brand
             << "\nDescription: " << description
             << "\nCurrent location: " << currentLocation
             << "\nOccurred at: " << occurredAt << '\n';
    }

public:
    Item(const string& idValue, const string& categoryValue, const string& monthValue, const string& statusValue, const string& kindValue, const string& colorValue, const string& brandValue, const string& descriptionValue, const string& currentLocationValue, const string& occurredAtValue)
        : id(idValue), category(categoryValue), month(monthValue), status(statusValue), kind(kindValue), color(colorValue), brand(brandValue), description(descriptionValue), currentLocation(currentLocationValue), occurredAt(occurredAtValue) {}

    virtual ~Item() {}

    Item(const Item&) = delete;
    Item& operator=(const Item&) = delete;

    string getPruningKey() const {
        return kind + "|" + category + "|" + month + "|" + color;
    }

    void setCurrentLocation(const string& newLocation) {
        currentLocation = newLocation;
    }

    void setOccurredAt(const string& newOccurredAt) {
        occurredAt = newOccurredAt;
    }

    virtual void displayDetails() const = 0;

    const string& getId() const { return id; }
    const string& getCategory() const { return category; }
    const string& getMonth() const { return month; }
    const string& getStatus() const { return status; }
    const string& getKind() const { return kind; }
    const string& getColor() const { return color; }
    const string& getBrand() const { return brand; }
    const string& getDescription() const { return description; }
    const string& getCurrentLocation() const { return currentLocation; }
    const string& getOccurredAt() const { return occurredAt; }
};

class LostItem : public Item {
private:
    string reporterContact;

public:
    LostItem(const string& id, const string& category, const string& month, const string& status, const string& color, const string& brand, const string& description, const string& currentLocation, const string& occurredAt, const string& reporterContact)
        : Item(id, category, month, status, "lost", color, brand, description, currentLocation, occurredAt), reporterContact(reporterContact) {}

    void displayDetails() const override {
        cout << "\n--- Lost item ---\n";
        displayCommonDetails();
        cout << "Reporter contact: " << reporterContact << '\n';
    }
};

class FoundItem : public Item {
private:
    string dropOffLocation;

public:
    FoundItem(const string& id, const string& category, const string& month,
              const string& status, const string& color, const string& brand,
              const string& description, const string& currentLocation,
              const string& occurredAt, const string& dropOffLocation)
        : Item(id, category, month, status, "found", color, brand, description,
               currentLocation, occurredAt),
          dropOffLocation(dropOffLocation) {}

    void displayDetails() const override {
        cout << "\n--- Found item ---\n";
        displayCommonDetails();
        cout << "Drop-off location: " << dropOffLocation << '\n';
    }
};

int main() {
    LostItem lostWallet(
        "L001", "accessory", "September", "Reported", "black", "Fossil",
        "black leather wallet with cards inside", "Library",
        "2026-09-18 10:30", "aly@geu.ac.in");

    FoundItem foundWallet(
        "F001", "accessory", "September", "Found", "black", "Fossil",
        "black leather wallet with cards inside", "Library entrance",
        "2026-09-18 11:15", "Library front desk");

    vector<Item*> items = {&lostWallet, &foundWallet};
    cout << fixed << setprecision(0);

    for (const Item* item : items) {
        item->displayDetails();
        cout << "Pruning key: " << item->getPruningKey() << '\n';
    }

    return 0;
}
