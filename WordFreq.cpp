#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"

WordFreq::WordFreq(std::string fileName) {
    this->fileName = fileName;
    this->wordFreqs = {};
    std::cout << "Initialization complete." << '\n';
}

void WordFreq::countWordFreqs() {
    std::string text;
    std::fstream myFile(fileName);

    while(getline(myFile, text, ' ')) {

        if(wordFreqs.find(text) != wordFreqs.end()) {
            wordFreqs[text] += 1;
        } else {
            wordFreqs[text] = 1;
        }
    }

    
}
