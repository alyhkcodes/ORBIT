#ifndef SCORING_H
#define SCORING_H

#include <string>
#include <unordered_map>
#include <vector>

std::vector<std::string> tokenize(const std::string& text);

struct Posting {
    int docID;
    int frequency;
};

class InvertedIndex {
private:
    std::unordered_map<std::string, std::vector<Posting>> index;

public:
    void build(const std::vector<std::vector<std::string>>& documents);
    const std::vector<Posting>& getPostings(const std::string& term) const;
    int docFrequency(const std::string& term) const;
};

class BM25 {
private:
    std::vector<std::vector<std::string>> documents;
    double avgDocLength;
    double k1;
    double b;
    InvertedIndex invertedIndex;

public:
    BM25(const std::vector<std::string>& documents,
         double k1 = 1.2,
         double b = 0.75);

    double idf(const std::string& term) const;
    std::unordered_map<int, double> score(const std::string& query) const;
};

#endif
