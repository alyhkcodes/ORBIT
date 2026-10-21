#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "item.h"

using namespace std;

namespace orbit {

struct ScoringCorpus {
    unordered_map<string, int> documentFrequency;
    int documentCount = 0;
    double averageLength = 0.0;
};

struct MatchScore {
    double total;
    double bm25;
};

double bm25Score(const unordered_map<string, int>& termFrequency,
                 int documentLength, const vector<string>& query,
                 const ScoringCorpus& corpus);

MatchScore scoreMatch(const Item& lost, const Item& found,
                      const unordered_map<string, int>& termFrequency,
                      int documentLength, const vector<string>& query,
                      const ScoringCorpus& corpus);

}  // namespace orbit
