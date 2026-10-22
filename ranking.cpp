#include <iostream>
#include <vector>
#include <queue>
#include "scoring.cpp"

using namespace std;

struct RankedItem {
    int id;
    double score;
    string text;

    // For priority_queue : highest score at the top
    bool operator<(const RankedItem& other) const {
        return score < other.score;
    }
};

int main() {
    // Example corpus
    vector<string> items;
    items.push_back("black samsung phone delhi");
    items.push_back("red nike shoes mumbai");
    items.push_back("blue backpack bangalore");

    BM25 bm25(items);
    string query = "black phone delhi";

    priority_queue<RankedItem> pq;

    // Compute BM25 scores for each item
    for (int i = 0; i < (int)items.size(); i++) {
        double s = bm25.score(query, i);   // BM25 score for query vs document i
        RankedItem ri;
        ri.id = i;
        ri.score = s;
        ri.text = items[i];
        pq.push(ri);
    }

    cout << "Ranking results for query: " << query << endl;

    // Pop items from priority queue in descending order
    while (!pq.empty()) {
        RankedItem top = pq.top();
        pq.pop();
        cout << "Item " << top.id << " | Score: " << top.score
             << " | Text: " << top.text << endl;
    }

    return 0;
}
