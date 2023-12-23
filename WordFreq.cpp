#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"

WordFreq::WordFreq(std::string fileName)
{
    this->fileName = fileName;
    this->wordFreqs = {};
    std::cout << "Initialization complete." << '\n';
}

void WordFreq::countWordFreqs(int head)
{
    std::string text;
    std::fstream myFile(fileName);
    if (head == -1)
    {
        while (getline(myFile, text, ' '))
        {

            if (wordFreqs.find(text) != wordFreqs.end())
            {
                wordFreqs[text] += 1;
            }
            else
            {
                wordFreqs[text] = 1;
            }
        }
    }
    else
    {
        for (int i = 0; i < head; i++)
        {
            if (!getline(myFile, text, ' '))
            {
                break;
            }

            if (wordFreqs.find(text) != wordFreqs.end())
            {
                wordFreqs[text] += 1;
            }
            else
            {
                wordFreqs[text] = 1;
            }
        }
    }
}

void WordFreq::printWordFreqs()
{
    for (auto &it : wordFreqs)
    {
        std::cout << it.first << " " << it.second << '\n';
    }
}

void WordFreq::sortWordFreqs()
{
    auto cmp = [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b)
    {
        return (a.second > b.second);
    };
    sortedWordFreq.assign(wordFreqs.begin(), wordFreqs.end());
    sort(sortedWordFreq.begin(), sortedWordFreq.end(), cmp);
}
