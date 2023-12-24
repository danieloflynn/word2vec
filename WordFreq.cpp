#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "WordFreq.h"

WordFreq::WordFreq(std::string fileName)
{
    this->fileName = fileName;
    this->wordFreqs = {};
}

// Returns false if char is a num, letter, or -, true otherwise
inline int WordFreq::isPunctuation(char &text)
{
    // if (text == '-')
    // {
    //     return 0;
    // }
    return ispunct(text);
}

// Remove unnecessary punctuation from text and make it lowercase
void WordFreq::cleanText(std::string &text)
{
    text.erase(std::remove_if(text.begin(), text.end(), ::ispunct), text.end());
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
}

void WordFreq::countFreqFromFile(int head)
{
    std::string text;
    std::fstream myFile(fileName);
    if (head == -1)
    {
        while (getline(myFile, text))
        {
            countFreq(text);
        }
    }
    else
    {
        for (int i = 0; i < head; i++)
        {
            if (!getline(myFile, text))
            {
                break;
            }
            countFreq(text);
        }
    }

    std::cout << "Finished counting word frequencies." << '\n';
}

void WordFreq::countFreq(std::string text)
{
    std::stringstream ss(text);
    while (getline(ss, text, ' '))
    {
        cleanText(text);

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

void WordFreq::serialize(std::string fileName)
{
    if (empty(sortedWordFreq))
    {
        sortWordFreqs();
    }
    std::ofstream newFile(fileName);
    for (auto &word : sortedWordFreq)
    {
        if (word.first != "")
        {
            newFile << word.first << " " << word.second << '\n';
        }
    }
}
