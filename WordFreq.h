#ifndef WORDFREQ_H
#define WORDFREQ_H

#include <string>
#include <vector>
#include <unordered_map>

class WordFreq
{
public:
    WordFreq(std::string fileName);
    std::string fileName;
    std::unordered_map<std::string, int> wordFreqs;
    std::vector<std::pair<std::string, int>> sortedWordFreq;
    void countFreqFromFile(int head = -1);
    void countFreq(std::string text);
    void printWordFreqs();
    void sortWordFreqs();
    void serialize(std::string fileName);
};

#endif