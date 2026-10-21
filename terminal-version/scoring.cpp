#include "scoring.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <unordered_map>

using namespace std;

// Tokenize into lowercase words and ignore punctuation.
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    string current;

    for (unsigned char ch : text) {
        if (isalnum(ch)) {
            current += static_cast<char>(tolower(ch));
        } else if (!current.empty()) {
            tokens.push_back(current);
            current.clear();
        }
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

void InvertedIndex::build(const vector<vector<string>>& documents) {
    index.clear();

    for (int docID = 0; docID < static_cast<int>(documents.size()); docID++) {
        unordered_map<string, int> frequency;

        for (const string& term : documents[docID]) {
            frequency[term]++;
        }

        for (const auto& entry : frequency) {
            index[entry.first].push_back({docID, entry.second});
        }
    }
}

const vector<Posting>& InvertedIndex::getPostings(const string& term) const {
    static const vector<Posting> emptyPostings;
    auto it = index.find(term);

    if (it == index.end()) {
        return emptyPostings;
    }

    return it->second;
}

int InvertedIndex::docFrequency(const string& term) const {
    auto it = index.find(term);

    if (it == index.end()) {
        return 0;
    }

    return static_cast<int>(it->second.size());
}

BM25::BM25(const vector<string>& inputDocuments, double k1, double b)
    : avgDocLength(0.0), k1(k1), b(b) {
    size_t totalLength = 0;

    for (const string& document : inputDocuments) {
        documents.push_back(tokenize(document));
        totalLength += documents.back().size();
    }

    // Avoid division by zero if there are no documents.
    if (!documents.empty()) {
        avgDocLength = static_cast<double>(totalLength) / documents.size();
    }

    invertedIndex.build(documents);
}

double BM25::idf(const string& term) const {
    const double N = static_cast<double>(documents.size());
    const double df = static_cast<double>(invertedIndex.docFrequency(term));

    if (N == 0.0) {
        return 0.0;
    }

    // Positive IDF variant commonly used by BM25.
    return log(1.0 + (N - df + 0.5) / (df + 0.5));
}

unordered_map<int, double> BM25::score(const string& query) const {
    unordered_map<int, double> scores;
    vector<string> queryTerms = tokenize(query);

    if (documents.empty() || avgDocLength == 0.0) {
        return scores;
    }

    for (const string& term : queryTerms) {
        const vector<Posting>& postings = invertedIndex.getPostings(term);
        const double inverseDocumentFrequency = idf(term);

        for (const Posting& posting : postings) {
            const int docID = posting.docID;
            const double frequency = static_cast<double>(posting.frequency);
            const double documentLength = static_cast<double>(documents[docID].size());

            const double denominator =
                frequency + k1 * (1.0 - b + b * documentLength / avgDocLength);

            if (denominator != 0.0) {
                scores[docID] += inverseDocumentFrequency *
                    (frequency * (k1 + 1.0)) / denominator;
            }
        }
    }

    return scores;
}
