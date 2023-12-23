#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <random>

#include "Word2Vec.h"

Word2Vec::Word2Vec(std::string fileName, std::string parseType, int minCount, int dimension)
{
    this->dimension = dimension;
    // Get a dictionary of words, words have greater count than minCount specified
    std::fstream myFile(fileName);
    std::string text;
    std::string count;
    if (parseType == "wordFreq")
    {
        while (getline(myFile, text, ' '))
        {
            getline(myFile, count);
            if (stoi(count) >= minCount)
            {
                dictionary.push_back(text);
                wordFreqs.push_back({text, stoi(count)});
            }
        }
    }
}

/*
Makes random word vectors for each word in the dictionary
These will be optimized in training
*/
void Word2Vec::makeRandomWordVecs()
{
    std::uniform_real_distribution<double> unif(vec_lower_bound, vec_upper_bound);
    std::default_random_engine re;
    for (std::string word : dictionary)
    {
        std::vector<double> wordVec;
        for (int i = 0; i < dimension; i++)
        {
            double random_double = unif(re);
            wordVec.push_back(random_double);
        }
        wordVecs[word] = wordVec;
    }
}

/*
Makes a vector of unigram frequencies of each word to be used in picking noise words.
Per the book, probability of picking a word:

P_alpha(word_i) = count(word_i)^alpha/sum(count(word_i)^alpha for all words)

*/

void Word2Vec::makeUnigramFreqs(double alpha)
{
    double totalFreq = 0;
    std::vector<double> freqs;
    // Add freq to the power of alpha to unigram freqs
    for (std::pair<std::string, int> word : wordFreqs)
    {
        unigram_freqs.push_back({word.first, pow(word.second, alpha)});
        totalFreq += pow(word.second, alpha);
    }

    // Now divide every element by the sum of all counts to the power of alpha for normalization
    std::cout << "Old word 1: " << unigram_freqs[0].second << '\n';
    for (std::pair<std::string, double> &word : unigram_freqs)
    {
        word.second /= totalFreq;
        freqs.push_back(word.second);
    }

    // Create a discrete_distribution for use in getRandomWord
    std::discrete_distribution<> dist(freqs.begin(), freqs.end());
    unigram_dist = dist;
}

std::random_device Word2Vec::rd;

/*
Returns a random word, probability freq based on the unigram distribution described above.
*/
std::string Word2Vec::getRandomWord()
{
    // need to subtract 1 because unigram dist goes from 1 - dict.size().
    return dictionary[unigram_dist(rd) - 1];
}

void Word2Vec::train(std::string trainingText)
{
    // Read in text
    std::fstream myFile(trainingText);
    std::string text;
    std::string currWord;
    std::vector<std::string> prevWords;
    std::vector<std::string> nextWords;

    // // Get the first words for the window size
    // for (int i = 0; i < window_size; i++)
    // {
    //     getline(myFile, text, ' ');
    //     slidingWindow.push_back(text);
    // }
}