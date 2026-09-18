#ifndef LOSTITEM_H
#define LOSTITEM_H

#include "item.h"

class LostItem : public Item
{
 private:
    string reporterContact;

 public:

    LostItem(string id, string category, string color, string brand,
              string description, string location, string timestamp,
              string reporterContact)
        : Item(id, category, color, brand, description, location, timestamp)
    {
        this->reporterContact = reporterContact;
    }

    string getReporterContact()
    {
        return reporterContact;
    }

    void printInfo() override 
    {
        cout << "[LOST] ";
        Item::printInfo();
    }
};

#endif