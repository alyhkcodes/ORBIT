#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "item.h"
#include "Scoring.h"

using namespace std;

namespace orbit {

struct Document {
    const Item* item;
    vector<string> tokens;
    unordered_map<string, int> termFrequency;
};

class InvertedIndex {
private:
    vector<Document> documents_;
    unordered_map<string, int> documentFrequency_;
    double averageLength_ = 0.0;

public:
    explicit InvertedIndex(const vector<Item*>& items);
    const vector<Document>& documents() const;
    ScoringCorpus scoringCorpus() const;
};

vector<string> tokenize(const string& text);

InvertedIndex::InvertedIndex(const vector<Item*>& items) {
    int totalLength = 0;
    for (int i = 0; i < items.size(); ++i) {
        Item* item = items[i];
        Document document{item, tokenize(item->description() + " " + item->brand()), {}};

        for (int j = 0; j < document.tokens.size(); ++j) {
            ++document.termFrequency[document.tokens[j]];
        }
        totalLength += document.tokens.size();
        for (auto term = document.termFrequency.begin(); term != document.termFrequency.end(); ++term) {
            ++documentFrequency_[term->first];
        }
        documents_.push_back(document);
    }
    averageLength_ = documents_.empty() ? 0.0 : static_cast<double>(totalLength) / documents_.size();
}

const vector<Document>& InvertedIndex::documents() const { return documents_; }

ScoringCorpus InvertedIndex::scoringCorpus() const {
    return {documentFrequency_, documents_.size(), averageLength_};
}

}  // namespace orbit
