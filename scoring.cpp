#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <algorithm>
using namespace std;

// Function to split a document into words
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    string word = "";
    for (int i = 0; i < (int)text.size(); i++) {
        char c = text[i];
        if (isspace(c)) {
            if (!word.empty()) {
                tokens.push_back(word);
                word = "";
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) tokens.push_back(word);
    return tokens;
}

// Posting structure for inverted index
struct Posting {
    int docID;
    int frequency;
};

// Inverted Index class
class InvertedIndex {
public:
    unordered_map<string, vector<Posting>> index;

    void build(const vector<string>& documents) {
        for (int docID = 0; docID < (int)documents.size(); docID++) {
            vector<string> tokens = tokenize(documents[docID]);
            unordered_map<string, int> termCount;

            for (int i = 0; i < (int)tokens.size(); i++) {
                string term = tokens[i];
                termCount[term]++;
            }

            for (auto it = termCount.begin(); it != termCount.end(); ++it) {
                string term = it->first;
                int freq = it->second;
                index[term].push_back({docID, freq});
            }
        }
    }

    vector<Posting> getPostings(const string& term) {
        if (index.find(term) != index.end()) {
            return index[term];
        }
        return vector<Posting>();
    }

    int docFrequency(const string& term) {
        if (index.find(term) != index.end()) {
            return (int)index[term].size();
        }
        return 0;
    }
};

// BM25 class
class BM25 {
    vector<vector<string>> corpus;      // All documents, tokenized
    double avgDocLength;               // Average document length
    double k1;                         // Saturation parameter
    double b;                          // Length normalization parameter
    InvertedIndex invertedIndex;

public:
    BM25(const vector<string>& documents, double k1Value = 1.2, double bValue = 0.75) {
        k1 = k1Value;
        b = bValue;
        int totalLength = 0;

        for (int i = 0; i < (int)documents.size(); i++) {
            vector<string> tokens = tokenize(documents[i]);
            corpus.push_back(tokens);
            totalLength += (int)tokens.size();
        }

        avgDocLength = (double)totalLength / documents.size();
        invertedIndex.build(documents);
    }

    double idf(const string& term) {
        int N = (int)corpus.size();
        int df = invertedIndex.docFrequency(term);
        return log((N - df + 0.5) / (df + 0.5) + 1);    // BM25 IDF calculation
    }

    // Compute BM25 score for a query against all document 
    unordered_map<int, double> score(const string& query) {
        vector<string> queryTokens = tokenize(query);
        unordered_map<int, double> scores;

        for (int i = 0; i < (int)queryTokens.size(); i++) {
            string term = queryTokens[i];
            vector<Posting> postings = invertedIndex.getPostings(term);
            double idfValue = idf(term);

            for (int j = 0; j < (int)postings.size(); j++) {
                int docID = postings[j].docID;
                int docLen = (int)corpus[docID].size();
                double f = postings[j].frequency;

                double numerator = f * (k1 + 1);
                double denominator = f + k1 * (1 - b + b * ((double)docLen / avgDocLength));
                double termScore = idfValue * (numerator / denominator);
                scores[docID] += termScore;
            }
        }
        return scores;
    }
};

// Example
int main() {
    vector<string> docs;
    docs.push_back("black leather wallet found near park");
    docs.push_back("lost mobile phone with blue cover");
    docs.push_back("brown purse found at bus stop");

    BM25 bm(docs);

    string query = "lost wallet";
    unordered_map<int, double> scores = bm.score(query);

    // Convert map to vector for sorting
    vector<pair<int, double>> ranked;
    for (auto it = scores.begin(); it != scores.end(); ++it) {
        ranked.push_back(make_pair(it->first, it->second));
    }

    // Sort results by score
    sort(ranked.begin(), ranked.end(), [](const pair<int, double>& a, const pair<int, double>& b) {
        return a.second > b.second;
    });

    for (int i = 0; i < (int)ranked.size(); i++) {
        cout << "Doc " << ranked[i].first << " score: " << ranked[i].second << endl;
    }
    return 0;
}
