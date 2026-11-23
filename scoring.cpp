#include "scoring.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <sstream>

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream input(text);
    std::string token;

    while (input >> token) {
        token.erase(std::remove_if(token.begin(), token.end(), [](unsigned char c) {
            return !std::isalnum(c);
        }), token.end());
        std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        if (!token.empty()) tokens.push_back(token);
    }
    return tokens;
}

void InvertedIndex::build(const std::vector<std::vector<std::string>>& documents) {
    index.clear();
    for (int docID = 0; docID < static_cast<int>(documents.size()); ++docID) {
        std::unordered_map<std::string, int> frequencies;
        for (const auto& term : documents[docID]) ++frequencies[term];
        for (const auto& entry : frequencies) {
            index[entry.first].push_back({docID, entry.second});
        }
    }
}

const std::vector<Posting>& InvertedIndex::getPostings(const std::string& term) const {
    static const std::vector<Posting> empty;
    const auto found = index.find(term);
    return found == index.end() ? empty : found->second;
}

int InvertedIndex::docFrequency(const std::string& term) const {
    const auto found = index.find(term);
    return found == index.end() ? 0 : static_cast<int>(found->second.size());
}

BM25::BM25(const std::vector<std::string>& sourceDocuments, double k1Value, double bValue)
    : avgDocLength(0.0), k1(k1Value), b(bValue) {
    std::vector<std::vector<std::string>> documents;
    for (const auto& document : sourceDocuments) {
        documents.push_back(tokenize(document));
        avgDocLength += documents.back().size();
    }
    if (!documents.empty()) avgDocLength /= documents.size();
    this->documents = std::move(documents);
    invertedIndex.build(this->documents);
}

double BM25::idf(const std::string& term) const {
    const double count = static_cast<double>(documents.size());
    const double frequency = invertedIndex.docFrequency(term);
    return std::log(1.0 + (count - frequency + 0.5) / (frequency + 0.5));
}

std::unordered_map<int, double> BM25::score(const std::string& query) const {
    std::unordered_map<int, double> scores;
    if (documents.empty() || avgDocLength == 0.0) return scores;

    for (const auto& term : tokenize(query)) {
        const double termIdf = idf(term);
        for (const auto& posting : invertedIndex.getPostings(term)) {
            const double length = documents[posting.docID].size();
            const double frequency = posting.frequency;
            const double denominator = frequency + k1 * (1.0 - b + b * length / avgDocLength);
            scores[posting.docID] += termIdf * frequency * (k1 + 1.0) / denominator;
        }
    }
    return scores;
}
