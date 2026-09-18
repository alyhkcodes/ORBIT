
#include <iostream>
#include <vector>
#include "item.h"
#include "lost_item.h"
#include "founditem.h"
using namespace std;

int main()
{
    LostItem wallet("L001", "wallet", "black", "Fossil",
                     "black leather wallet with cards inside",
                     "Library", "2026-09-18 10:30",
                     "aly@geu.ac.in");

    FoundItem foundWallet("F001", "wallet", "dark brown", "Fossil",
                           "dark leather wallet found near entrance",
                           "Library", "2026-09-18 11:15",
                           "priyanshu@geu.ac.in", "Library front desk");

    wallet.printInfo();
    foundWallet.printInfo();

    vector<Item*> allItems;
    allItems.push_back(&wallet);
    allItems.push_back(&foundWallet);

    cout << "\n--- Looping through as Item* ---\n";
    for (Item* it : allItems) {
        it->printInfo();
    }

    return 0;
}