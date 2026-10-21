#include "Scoring.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

using namespace std;

namespace orbit {

// Function Pre-Decleration
string normalize(const string& value);
vector<string> tokenize(const string& text);

namespace {

double tokenOverlap(const string& first, const string& second) {
    //Tokenizing
    const auto leftTokens = tokenize(first);
    const auto rightTokens = tokenize(second);

    const unordered_set<string> left(leftTokens.begin(), leftTokens.end());
    const unordered_set<string> right(rightTokens.begin(), rightTokens.end());
    if (left.empty() || right.empty()) return 0.0;

    int common = 0;
    for (auto token = left.begin(); token != left.end(); ++token) {
        if (right.count(*token)) ++common;
    }
    return static_cast<double>(common) / (left.size() + right.size() - common);
}

}  // namespace

double bm25Score(const unordered_map<string, int>& termFrequency,
                 int documentLength, const vector<string>& query,
                 const ScoringCorpus& corpus) {
    constexpr double k1 = 1.5;
    constexpr double b = 0.75;
    double result = 0.0;
    for (int i = 0; i < static_cast<int>(query.size()); ++i) {
        const string& term = query[i];
        const auto tf = termFrequency.find(term);
        if (tf == termFrequency.end()) continue;

        const auto df = corpus.documentFrequency.find(term);
        const double documentFrequency = df == corpus.documentFrequency.end() ? 0.0 : static_cast<double>(df->second);

        const double idf = log(1.0 + (corpus.documentCount - documentFrequency + 0.5) / (documentFrequency + 0.5));
        
        const double length = static_cast<double>(documentLength);
        const double denominator = tf->second +
            k1 * (1.0 - b + b * length / max(corpus.averageLength, 1.0));
        result += idf * (tf->second * (k1 + 1.0)) / denominator;
    }
    return result;
}

MatchScore scoreMatch(const Item& lost, const Item& found, const unordered_map<string, int>& termFrequency, int documentLength, const vector<string>& query, const ScoringCorpus& corpus) {
    const double bm25 = bm25Score(termFrequency, documentLength, query, corpus);

    const double textScore = bm25 / (bm25 + 5.0);

    const double colorScore = normalize(lost.color()) == normalize(found.color()) ? 1.0 : 0.0;

    const double brandScore = tokenOverlap(lost.brand(), found.brand());

    const double locationScore = tokenOverlap(lost.location(), found.location());

    const double daysApart = abs(static_cast<double>(lost.occurredAt() - found.occurredAt())) / 86400.0;
    const double timeScore = exp(-daysApart / 30.0);
    const double total = 100.0 * (0.60 * textScore + 0.15 * colorScore + 0.10 * brandScore + 0.075 * locationScore + 0.075 * timeScore);

    return {total, bm25};
}

}  // namespace orbit
