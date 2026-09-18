#ifndef FOUNDITEM_H
#define FOUNDITEM_H
#include "item.h"

class FoundItem : public Item
{
private:
        string finderContact;
        string currentHoldingLocation;

public:
        FoundItem(string id, string category, string color, string brand,
               string description, string location, string timestamp,
               string finderContact, string currentHoldingLocation) : Item(id, category, color, brand, description, location, timestamp) 
               {
                this->finderContact = finderContact;
                this->currentHoldingLocation = currentHoldingLocation;
               }
    string getFinderContact() {
        return finderContact;
    }

    string getCurrentHoldingLocation() {
        return currentHoldingLocation;
    }

    void printInfo() override {
        cout << "[FOUND] ";
        Item::printInfo();
        cout << "  Currently held at: " << currentHoldingLocation << endl;
    }
};

#endif
