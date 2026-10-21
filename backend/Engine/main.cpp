#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>

#include "ExceptionHandler.h"

using namespace std;
using namespace nlohmann;
namespace fs = std::filesystem;

namespace {

const fs::path reportsPath = fs::path("data") / "reports.json";

json loadReports() {
    if (!fs::exists(reportsPath)) return {{"lostItems", json::array()}, {"foundItems", json::array()}};
    ifstream input(reportsPath);
    if (!input) throw runtime_error("Could not open " + reportsPath.string());
    json reports;
    input >> reports;
    if (!reports.is_object() || !reports.value("lostItems", json()).is_array() || !reports.value("foundItems", json()).is_array()) {
        throw runtime_error("Saved reports must contain lostItems and foundItems arrays.");
    }
    return reports;
}

void saveReports(const json& reports) {
    fs::create_directories(reportsPath.parent_path());
    ofstream output(reportsPath, ios::trunc);
    if (!output) throw runtime_error("Could not write " + reportsPath.string());
    output << reports.dump(2) << '\n';
    if (!output) throw runtime_error("Failed while writing " + reportsPath.string());
}

string prompt(const string& label, bool optional = false) {
    while (true) {
        cout << label << (optional ? " (optional)" : "") << ": ";
        string value;
        if (!getline(cin, value)) throw runtime_error("Input ended.");
        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == string::npos) {
            if (optional) return "";
            cout << "This field is required.\n";
            continue;
        }
        return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
    }
}

int promptMonth() {
    while (true) {
        const string value = prompt("Month number (1-12)");
        try {
            size_t used = 0;
            int month = stoi(value, &used);
            if (used == value.size() && month >= 1 && month <= 12) return month;
        } catch (const exception&) {}
        cout << "Enter a whole number from 1 to 12.\n";
    }
}

string nextId(const json& reports) {
    size_t count = reports["lostItems"].size() + reports["foundItems"].size() + 1;
    ostringstream id;
    id << "ORBIT-" << setw(6) << setfill('0') << count;
    return id.str();
}

json promptReport(const string& kind, const json& reports) {
    cout << "\nNew " << (kind == "LOST" ? "lost" : "found") << " report\n";
    json item = {
        {"id", nextId(reports)}, {"kind", kind},
        {"category", prompt("Category")}, {"month", promptMonth()},
        {"color", prompt("Color")}, {"brand", prompt("Brand", true)},
        {"description", prompt("Description")}, {"location", prompt("Where it was lost/found")},
        {"occurredAt", prompt("Date (YYYY-MM-DD or ISO date-time)")}, {"status", "REPORTED"}
    };
    if (kind == "LOST") item["reporterContact"] = prompt("Your contact");
    else {
        item["finderContact"] = prompt("Your contact");
        item["currentLocation"] = prompt("Where the item is now");
    }
    return item;
}

void showReports(const json& reports) {
    cout << "\nSaved reports\n";
    bool any = false;
    for (const char* key : {"lostItems", "foundItems"}) {
        for (const auto& item : reports[key]) {
            any = true;
            cout << "- " << item.value("id", "?") << " [" << item.value("kind", "?") << "] "
                 << item.value("category", "") << ", " << item.value("color", "")
                 << " — " << item.value("description", "") << " (" << item.value("occurredAt", "") << ")\n";
        }
    }
    if (!any) cout << "No reports yet.\n";
}

const json* findItem(const json& reports, const string& id) {
    for (const char* key : {"lostItems", "foundItems"})
        for (const auto& item : reports[key])
            if (item.value("id", "") == id) return &item;
    return nullptr;
}

void showMatches(const json& reports) {
    json request = {{"lostItems", reports["lostItems"]}, {"foundItems", reports["foundItems"]}, {"topK", 5}};
    const json response = orbit::handleRequestLine(request.dump());
    if (response.contains("error")) throw runtime_error(response["error"].value("message", "Matching failed."));
    const auto& matches = response["matches"];
    cout << "\nTop matches\n";
    if (matches.empty()) {
        cout << "No compatible matches found yet.\n";
        return;
    }
    for (const auto& match : matches) {
        const json* lost = findItem(reports, match.value("lostId", ""));
        const json* found = findItem(reports, match.value("foundId", ""));
        cout << fixed << setprecision(2) << "Score " << match.value("score", 0.0) << " | ";
        if (lost) cout << "Lost: " << lost->value("description", "") << " at " << lost->value("location", "");
        if (found) cout << " | Found: " << found->value("description", "") << " at " << found->value("currentLocation", "");
        cout << '\n';
    }
}

int runCli() {
    json reports = loadReports();
    cout << "ORBIT Lost & Found\nReports are saved in " << reportsPath.string() << ".\n";
    while (true) {
        cout << "\n1. Add lost report\n2. Add found report\n3. List reports\n4. Exit\n";
        const string choice = prompt("Choose an option");
        if (choice == "1" || choice == "2") {
            json item = promptReport(choice == "1" ? "LOST" : "FOUND", reports);
            reports[choice == "1" ? "lostItems" : "foundItems"].push_back(move(item));
            const json validation = orbit::handleRequestLine(json({
                {"lostItems", reports["lostItems"]},
                {"foundItems", reports["foundItems"]},
                {"topK", 0}
            }).dump());
            if (validation.contains("error")) {
                reports[choice == "1" ? "lostItems" : "foundItems"].erase(
                    reports[choice == "1" ? "lostItems" : "foundItems"].size() - 1);
                cout << "Report was not saved: "
                     << validation["error"].value("message", "Invalid report.") << '\n';
                continue;
            }
            saveReports(reports);
            cout << "Report saved.\n";
            showMatches(reports);
        } else if (choice == "3") {
            showReports(reports);
        } else if (choice == "4") {
            return 0;
        } else {
            cout << "Choose 1, 2, 3, or 4.\n";
        }
    }
}

}  // namespace

int main() {
    try {
        return runCli();
    } catch (const json::exception& error) {
        cerr << "Saved report data is invalid: " << error.what() << '\n';
        return 1;
    } catch (const exception& error) {
        cerr << "ORBIT stopped: " << error.what() << '\n';
        return 1;
    }
}
