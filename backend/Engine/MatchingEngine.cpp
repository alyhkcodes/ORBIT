#include <algorithm>
#include <exception>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "item.h"
#include "Scoring.h"

using namespace std;
using namespace nlohmann;

namespace orbit {

struct Document {
    const Item* item;
    vector<string> tokens;
    unordered_map<string, int> termFrequency;
};

class InvertedIndex {
public:
    explicit InvertedIndex(const vector<Item*>& items);
    const vector<Document>& documents() const;
    ScoringCorpus scoringCorpus() const;

private:
    vector<Document> documents_;
    unordered_map<string, int> documentFrequency_;
    double averageLength_ = 0.0;
};

Item* parseItem(const json& value, const string& expectedKind);
string normalize(const string& value);
vector<string> tokenize(const string& text);

namespace {

struct Match {
    const Item* lost;
    const Item* found;
    double score;
    double bm25;
};

struct WorstMatchAtTop {
    bool operator()(const Match& left, const Match& right) const {
        if (left.score != right.score) return left.score > right.score;
        if (left.lost->id() != right.lost->id()) return left.lost->id() < right.lost->id();
        return left.found->id() < right.found->id();
    }
};

bool betterMatch(const Match& candidate, const Match& current) {
    if (candidate.score != current.score) return candidate.score > current.score;
    if (candidate.lost->id() != current.lost->id()) return candidate.lost->id() < current.lost->id();
    return candidate.found->id() < current.found->id();
}

}  // namespace

json matchRequest(const json& request) {
    if (!request.is_object()) throw invalid_argument("request must be a JSON object");
    if (!request.contains("lostItems") || !request["lostItems"].is_array()) {
        throw invalid_argument("'lostItems' must be an array");
    }
    if (!request.contains("foundItems") || !request["foundItems"].is_array()) {
        throw invalid_argument("'foundItems' must be an array");
    }

    int topK = 5;
    if (request.contains("topK")) {
        if (!request["topK"].is_number_integer()) throw invalid_argument("'topK' must be an integer");
        topK = request["topK"].get<int>();
        if (topK < 0 || topK > 100) throw invalid_argument("'topK' must be between 0 and 100");
    }

    vector<Item*> lostItems;
    vector<Item*> foundItems;
    for (int i = 0; i < request["lostItems"].size(); ++i) {
        lostItems.push_back(parseItem(request["lostItems"][i], "LOST"));
    }
    for (int i = 0; i < request["foundItems"].size(); ++i) {
        foundItems.push_back(parseItem(request["foundItems"][i], "FOUND"));
    }

    const InvertedIndex index(foundItems);
    const ScoringCorpus scoringCorpus = index.scoringCorpus();

    unordered_map<string, vector<const Item*>> foundByKey;
    unordered_map<const Item*, const Document*> documentByItem;

    for (int i = 0; i < index.documents().size(); ++i) {
        const Document& document = index.documents()[i];
        foundByKey[document.item->pruningKey()].push_back(document.item);
        documentByItem[document.item] = &document;
    }

    priority_queue<Match, vector<Match>, WorstMatchAtTop> topMatches;
    for (int i = 0; i < lostItems.size(); ++i) {
        Item* lost = lostItems[i];
        const auto bucket = foundByKey.find(lost->pruningKey());
        if (bucket == foundByKey.end() || topK == 0) continue;

        const auto queryTokens = tokenize(lost->description() + " " + lost->brand());
        const set<string> uniqueQuery(queryTokens.begin(), queryTokens.end());
        const vector<string> query(uniqueQuery.begin(), uniqueQuery.end());

        for (int j = 0; j < bucket->second.size(); ++j) {
            const Item* found = bucket->second[j];
            const Document& document = *documentByItem.at(found);
            const MatchScore score = scoreMatch(*lost, *found, document.termFrequency,
                                                static_cast<int>(document.tokens.size()), query,
                                                scoringCorpus);
            const Match candidate{lost, found, score.total, score.bm25};
            if (static_cast<int>(topMatches.size()) < topK) {
                topMatches.push(candidate);
            } else if (betterMatch(candidate, topMatches.top())) {
                topMatches.pop();
                topMatches.push(candidate);
            }
        }
    }

    vector<Match> ordered;
    while (!topMatches.empty()) {
        ordered.push_back(topMatches.top());
        topMatches.pop();
    }
    reverse(ordered.begin(), ordered.end());

    auto results = json::array();
    for (int i = 0; i < ordered.size(); ++i) {
        const Match& match = ordered[i];
        results.push_back({
            {"lostId", match.lost->id()},
            {"foundId", match.found->id()},
            {"score", round(match.score * 100.0) / 100.0},
            {"bm25Score", round(match.bm25 * 1000.0) / 1000.0}
        });
    }
    for (int i = 0; i < lostItems.size(); ++i) delete lostItems[i];
    for (int i = 0; i < foundItems.size(); ++i) delete foundItems[i];
    return {{"matches", move(results)}};
}

}  // namespace orbit
