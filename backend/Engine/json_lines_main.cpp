#include <iostream>
#include <string>

#include "ExceptionHandler.h"

using namespace std;
using namespace nlohmann;

int main() {
    string line;
    while (getline(cin, line)) {
        if (line.find_first_not_of(" \t\r\n") == string::npos) continue;
        cout << orbit::handleRequestLine(line).dump() << '\n';
    }
    return 0;
}
