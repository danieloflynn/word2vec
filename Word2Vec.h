#ifndef WORD2VEC_H
#define WORD2VEC_H

#include <string>
#include <vector>
#include <unordered_map>

class Word2Vec
{
public:
    Word2Vec(std::string fileName, std::string parseType = "wordFreq", int minCount = 1, int dimension = 512);
    int dimension;
    std::vector<std::string> dictionary;
    std::unordered_map<std::string, std::vector<double>> wordVecs;
    const double vec_lower_bound = -10000.0;
    const double vec_upper_bound = 10000.0;
    void makeRandomWordVecs();
};

#endif