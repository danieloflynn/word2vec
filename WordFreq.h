#ifndef WORDFREQ_H
#define WORDFREQ_H

#include <string>
#include <vector>
#include <unordered_map>

class WordFreq {
    public:
        WordFreq(std::string fileName);
        std::string fileName;
        std::unordered_map<std::string, int> wordFreqs;
        void countWordFreqs();

};







#endif