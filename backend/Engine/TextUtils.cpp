#include <algorithm>
#include <cctype>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace orbit {

string lower(string value) {
    transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return tolower(ch);
    });
    return value;
}

string trim(const string& value) {
    auto first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos) return {};
    auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

string normalize(const string& value) {
    return lower(trim(value));
}

vector<string> tokenize(const string& text) {
    vector<string> tokens;
    string token;
    string lowered = lower(text);
    for (int i = 0; i < lowered.size(); ++i) {
        unsigned char ch = lowered[i];
        if (isalnum(ch)) {
            token.push_back(ch);
        } else if (!token.empty()) {
            tokens.push_back(token);
            token.clear();
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

}  // namespace orbit
