#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"

WordFreq::WordFreq(std::string fileName)
{
    this->fileName = fileName;
    this->wordFreqs = {};
}

void WordFreq::countWordFreqs(int head)
{
    std::string text;
    std::fstream myFile(fileName);
    if (head == -1)
    {
        while (getline(myFile, text))
        {
            stringstream ss(text);
            while (getline(ss, text, ' '))
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

    std::cout << "Finished counting word frequencies." << '\n';
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
