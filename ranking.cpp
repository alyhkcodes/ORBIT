#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include "scoring.cpp"   // BM25 implementation

using namespace std;

struct RankedItem {
    int id;
    double score;
    string text;

    bool operator<(const RankedItem& other) const {
        return score < other.score; // max-heap
    }
};

int main() {
    // Example documents
    vector<string> items;
    items.push_back("black samsung phone delhi");
    items.push_back("red nike shoes mumbai");
    items.push_back("blue backpack bangalore");

    BM25 bm25(items);
    string query = "black phone delhi";

    // Get BM25 scores for all docs
    unordered_map<int, double> scores = bm25.score(query);

    // Rank results using a priority queue
    priority_queue<RankedItem> pq;

    for (unordered_map<int, double>::iterator it = scores.begin(); it != scores.end(); ++it) {
        RankedItem ri;
        ri.id = it->first;
        ri.score = it->second;
        ri.text = items[it->first];
        pq.push(ri);
    }

    cout << "Ranking results for query: " << query << endl;
    while (!pq.empty()) {
        RankedItem top = pq.top();
        pq.pop();
        cout << "Item " << top.id << " | Score: " << top.score
             << " | Text: " << top.text << endl;
    }

    return 0;
}
