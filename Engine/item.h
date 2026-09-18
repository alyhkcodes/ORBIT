#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>
using namespace std;

enum class Status {
    ACTIVE,
    MATCHED,
    RESOLVED
};

class Item {
protected:
    string id;
    string category;
    string color;
    string brand;
    string description;
    string location;
    string timestamp;
    Status status;

 public:
    Item(string id, string category, string color, string brand,
         string description, string location, string timestamp)
        {
         this->id = id;
         this->category = category;
         this->color = color;
         this->brand = brand;
         this->description = description;
         this->location = location;
         this->timestamp = timestamp;
         this->status = Status::ACTIVE;
        }

    string getId() { return id; }
    string getCategory() { return category; }
    string getDescription() { return description; }
    Status getStatus() { return status; }

    void setStatus(Status newStatus)
    {
        status = newStatus;
    }

    virtual void printInfo()
    {
        cout << "ID: " << id << ", Category: " << category
             << ", Description: " << description << endl;
    }

    virtual ~Item() {}
};

#endif