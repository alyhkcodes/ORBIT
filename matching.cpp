#include "matching.h"
#include "scoring.h"

#include <queue>
#include <string>
#include <vector>

using namespace std;

// A priority queue puts the highest score at the top by default.
struct MatchCompare {
    bool operator()(const MatchResult& a, const MatchResult& b) const {
        if (a.score == b.score) {
            return a.itemId > b.itemId;
        }
        return a.score < b.score;
    }
};

static string makeDocument(const Item& item) {
    return item.title + " " + item.category + " " +
           item.description + " " + item.location;
}

vector<MatchResult> findMatches(const Item& lostItem,
                                const vector<Item>& allItems,
                                int maximumResults) {
    vector<const Item*> candidates;
    vector<string> candidateDocuments;

    // Only compare a LOST item with unresolved FOUND items.
    for (const Item& item : allItems) {
        if (item.type == "FOUND" && !item.resolved && item.id != lostItem.id) {
            candidates.push_back(&item);
            candidateDocuments.push_back(makeDocument(item));
        }
    }

    vector<MatchResult> results;

    if (candidates.empty() || maximumResults <= 0) {
        return results;
    }

    // BM25 scores the found-item descriptions against the lost-item query.
    BM25 bm25(candidateDocuments);
    const string query = makeDocument(lostItem);
    unordered_map<int, double> scores = bm25.score(query);

    priority_queue<MatchResult, vector<MatchResult>, MatchCompare> ranked;

    for (int i = 0; i < static_cast<int>(candidates.size()); i++) {
        auto it = scores.find(i);
        double score = (it == scores.end()) ? 0.0 : it->second;

        // Ignore zero-score candidates because they share no searchable terms.
        if (score > 0.0) {
            ranked.push({candidates[i]->id, score});
        }
    }

    while (!ranked.empty() && static_cast<int>(results.size()) < maximumResults) {
        results.push_back(ranked.top());
        ranked.pop();
    }

    return results;
}
