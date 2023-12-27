#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <random>
#include <vector>

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
                dictSet.insert(text);
                wordFreqs.push_back({text, stoi(count)});
            }
        }
        std::cout << "Successfully read in freq dictionary." << '\n';
    }
}

// Returns false if char is a num, letter, or -, true otherwise
inline int Word2Vec::isPunctuation(char &text)
{
    // if(text == '-'){
    //     return 0;
    // }
    return ispunct(text);
}

// Remove unnecessary punctuation from text
void Word2Vec::cleanText(std::string &text)
{
    text.erase(std::remove_if(text.begin(), text.end(), ::ispunct), text.end());
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
}

/*
Makes random word and context vectors for each word in the dictionary
These will be optimized in training
*/
void Word2Vec::makeRandomVecs()
{
    std::cout << "Making random word and context vectors." << '\n';

    std::uniform_real_distribution<double> unif(vec_lower_bound, vec_upper_bound);
    std::default_random_engine re;
    for (std::string word : dictionary)
    {
        std::vector<double> wordVec;
        std::vector<double> contVec;
        for (int i = 0; i < dimension; i++)
        {
            double random_double = unif(re);
            wordVec.push_back(random_double);
            random_double = unif(re);
            contVec.push_back(random_double);
        }
        wordVecs[word] = wordVec;
        contextVecs[word] = contVec;
    }

    std::cout << "Finished making random word and context vectors." << '\n';
}

void Word2Vec::writeContextVecsToFile(std::string fileName)
{
    std::ofstream newFile(fileName);
    for (auto &word : contextVecs)
    {
        if (word.first != "")
        {
            newFile << word.first << " ";
            for (auto &vec : word.second)
            {
                newFile << vec << " ";
            }
            newFile << '\n';
        }
    }

    newFile.close();
}

void Word2Vec::writeWordVecsToFile(std::string fileName)
{
    std::ofstream newFile(fileName);
    for (auto &word : wordVecs)
    {
        if (word.first != "")
        {
            newFile << word.first << " ";
            for (auto &vec : word.second)
            {
                newFile << vec << " ";
            }
            newFile << '\n';
        }
    }

    newFile.close();
}

/*Reads in the context vectors from file
Context vec consists of word, followed vector of space delimited doubles
*/

void Word2Vec::readContextVecsFromFile(std::string fileName)
{
    std::cout << "Reading in context vectors..." << '\n';
    contextVecs = {}; // Clear context vecs
    std::fstream myFile(fileName);
    std::string text;
    std::string word;

    // Get line
    while (getline(myFile, text))
    {
        std::vector<double> vec;

        // First item in line is the word
        std::stringstream ss(text);
        getline(ss, word, ' ');
        if (dictSet.find(word) == dictSet.end())
        {
            // std::cout << word << " not in dict" << '\n';
            continue;
        }

        // Rest are the vector
        while (getline(ss, text, ' '))
        {
            vec.push_back(stod(text));
        }
        contextVecs[word] = vec;
    }
    std::cout << "Successfully read in context vectors." << '\n';
}

/*Reads in the word vectors from file
Word vec consists of word, followed vector of space delimited doubles
*/

void Word2Vec::readWordVecsFromFile(std::string fileName)
{
    std::cout << "Reading in word vectors..." << '\n';
    wordVecs = {};
    std::fstream myFile(fileName);
    std::string text;
    std::string word;

    // Get line
    while (getline(myFile, text))
    {
        std::vector<double> vec;

        // First item in line is the word
        std::stringstream ss(text);
        getline(ss, word, ' ');
        if (dictSet.find(word) == dictSet.end())
        {
            // std::cout << word << " not in dict" << '\n';
            continue;
        }

        // Rest are the vector
        while (getline(ss, text, ' '))
        {
            vec.push_back(stod(text));
        }
        wordVecs[word] = vec;
    }
    std::cout << "Successfully read in word vectors." << '\n';
}
/*
Makes a vector of unigram frequencies of each word to be used in picking noise words.
Per the book, probability of picking a word:

P_alpha(word_i) = count(word_i)^alpha/sum(count(word_i)^alpha for all words)

*/

void Word2Vec::makeUnigramFreqs(double alpha)
{
    std::cout << "Making unigram distributions" << '\n';
    double totalFreq = 0;
    std::vector<double> freqs;
    // Add freq to the power of alpha to unigram freqs
    for (std::pair<std::string, int> word : wordFreqs)
    {
        unigram_freqs.push_back({word.first, pow(word.second, alpha)});
        totalFreq += pow(word.second, alpha);
    }

    // Now divide every element by the sum of all counts to the power of alpha for normalization
    for (std::pair<std::string, double> &word : unigram_freqs)
    {
        word.second /= totalFreq;
        freqs.push_back(word.second);
    }

    // Create a discrete_distribution for use in getRandomWord
    std::discrete_distribution<> dist(freqs.begin(), freqs.end());
    unigram_dist = dist;
    std::cout << "Finished making unigram distributions" << '\n';
}

std::random_device Word2Vec::rd;

/*
Returns a random word, probability freq based on the unigram distribution described above.
*/
std::string Word2Vec::getRandomWord()
{

    return dictionary[unigram_dist(rd)];
}

// returns vector multiplied by scalar
std::vector<double> Word2Vec::scalarMult(std::vector<double> &vec, double scalar)
{
    std::vector<double> newVec;

    for (double &num : vec)
    {
        newVec.push_back(num * scalar);
    }

    return newVec;
}

// Add two vectors and return the result
std::vector<double> Word2Vec::vectorAdd(std::vector<double> &vec1, std::vector<double> &vec2)
{
    // If vectors are different sizes print error and return null vector
    if (vec1.size() != vec2.size())
    {
        std::cout << "Error: vectors are different sizes." << '\n';
        return {};
    }

    std::vector<double> newVec;

    for (int i = 0; i < vec1.size(); i++)
    {
        newVec.push_back(vec1[i] + vec2[i]);
    }

    return newVec;
}

// Gets the dot product of two vectors
double Word2Vec::dotProd(std::vector<double> &vec1, std::vector<double> &vec2)
{
    if (vec1.size() != vec2.size())
    {
        std::cout << "Error: vectors are different sizes." << '\n';
        return 0.0;
    }

    double total = 0;

    for (int i = 0; i < vec1.size(); i++)
    {
        total += vec1[i] * vec2[i];
    }

    return total;
}

/*
Returns sigmoid of the number.
sigmoid(x) = 1/(1+e^(-x))
*/
double Word2Vec::sigmoid(double num)
{
    return 1 / (1 + exp(-num));
}

/*
Returns a list of similar words to the target word.
Similarity is calculated via the sigmoid function.
*/
std::vector<std::pair<std::string, double>> Word2Vec::calcSimilarWords(std::string word)
{
    // Make sure word exists in dictionary
    if (dictSet.find(word) == dictSet.end())
    {
        std::cout << "Error: word not in dictionary" << '\n';
        return {};
    }

    std::vector<std::pair<std::string, double>> wordSimilarity;

    std::vector<double> wVec = wordVecs[word];

    for (auto w : wordVecs)
    {
        if (w.first == word)
        {
            continue;
        }

        double sim = dotProd(wVec, w.second);
        wordSimilarity.push_back({w.first, sim});
    }

    // Sort by similarity

    auto cmp = [](const std::pair<std::string, double> &a, const std::pair<std::string, double> &b)
    {
        return (a.second > b.second);
    };
    sort(wordSimilarity.begin(), wordSimilarity.end(), cmp);
    return wordSimilarity;
}

/*
Updates the positive context vector.
c_pos(t+1) = c_pos(t) - learn_rate * [sigmoid(c_neg(t)*w(t)) - 1] w(t)
*/
void Word2Vec::updateCPosVec(std::vector<double> &cPosVec, std::vector<double> &wVec)
{

    double scalar = -learning_rate * (sigmoid(dotProd(cPosVec, wVec)) - 1);
    std::vector<double> newVec = scalarMult(wVec, scalar);
    cPosVec = vectorAdd(cPosVec, newVec);
}

/*
Updates the negative context vector.
c_neg(t+1) = c_neg(t) - learn_rate * [sigmoid(c_pos(t)*w(t))] w(t)
*/
void Word2Vec::updateCNegVec(std::vector<double> &cNegVec, std::vector<double> &wVec)
{
    double scalar = -learning_rate * (sigmoid(dotProd(cNegVec, wVec)));
    std::vector<double> newVec = scalarMult(wVec, scalar);
    cNegVec = vectorAdd(cNegVec, newVec);
}

/*
Updates the word vector
w(t+1) = w(t) - learn_rate[ [sigmoid(c_pos(t)*w(t)) - 1] c_pos + sum_all_c_neg(sigmoid(c_neg(t)*w(t)) - 1) c_neg ]
*/
void Word2Vec::updateWVec(std::vector<double> &wVec, std::vector<double> &cPosVec, std::vector<std::vector<double> *> &cNegVecs)
{

    // Add the c_pos part
    double cPosScalar = -learning_rate * (sigmoid(dotProd(cPosVec, wVec)) - 1);
    std::vector<double> cPosVecPart = scalarMult(cPosVec, cPosScalar);
    std::vector<double> newWVec = vectorAdd(wVec, cPosVecPart); // Make new word vector as we need the old one to calculate cNegVecPart

    // add each cNeg vector to the new w vector
    for (std::vector<double> *&cNegVec : cNegVecs)
    {
        double cNegScalar = -learning_rate * (sigmoid(dotProd((*cNegVec), wVec)));
        std::vector<double> cNegVecPart = scalarMult(*cNegVec, cNegScalar);
        newWVec = vectorAdd(newWVec, cNegVecPart);
    }
    // Now change wVec to the new word vec
    wVec = newWVec;
}

/*
Takes a positive context vector and word vector.
Creates k negative context vector.
Updates the positive, negative and word vectors using the update equations.
*/
void Word2Vec::updateVectors(std::vector<double> &wVec, std::vector<double> &cPosVec)
{
    // First, get random negative context vectors.
    std::vector<std::vector<double> *> cNegVecs;
    for (int i = 0; i < ratio_neg_context_vectors; i++)
    {
        std::string randWord = getRandomWord();
        cNegVecs.push_back(&contextVecs[randWord]);
    }

    // update positive context vectors
    updateCPosVec(cPosVec, wVec);

    // update negative context vectors
    for (std::vector<double> *cNegVec : cNegVecs)
    {
        updateCNegVec(*cNegVec, wVec);
    }

    // Update the word vector
    updateWVec(wVec, cPosVec, cNegVecs);
}

/*
Implements the training for skip-gram embeddings.
Takes each line in the training corpus individually.
We look window_size words either side of our target word and treat these as our positive context vectors.
We then feed our positive context vector and word vector into the vector learning algorithm
This adds k random negative context vectors (random words from the corpus).
We update the target word vector to be closer to the positive context vectors and vice versa,
and also update the target word vector to be further away from the negative context vectors and vice versa.
*/
void Word2Vec::train(std::string trainingText, std::string cVecOutput, std::string wVecOutput)
{
    // Read in text
    std::fstream myFile(trainingText);
    std::string text;
    int lineCount = 0;
    bool breaking = false;
    // Get a line
    while (getline(myFile, text))
    {
        std::vector<std::string> words;
        std::stringstream ss(text);
        std::string currWord;
        std::vector<std::string> prevWords;
        std::vector<std::string> nextWords;

        std::cout << "Line " << lineCount << '\n';
        // Add all the words to a vector
        while (getline(ss, text, ' '))
        {
            cleanText(text);
            if (text.size() > 0)
            {

                words.push_back(text);
            }
        }

        // Check that at least 2 words are present
        if (words.size() < 2)
        {
            continue;
        }

        // Now iterate over the sliding window
        for (int i = 0; i < words.size(); i++)
        {
            // If current word there, add to prev words
            if (!currWord.empty())
            {
                prevWords.push_back(currWord);
            }
            // If prevWords vector is greater than the window size, remove the first element.
            if (prevWords.size() > window_size)
            {
                prevWords.erase(prevWords.begin());
            }

            // Set the current word to the next word in the sequence
            currWord = words[i];

            // Add extra words to the "next words" if we're at the start
            if (i == 0)
            {
                for (int j = 1; j < std::min(words.size() - 1, static_cast<size_t>(window_size)); j++)
                {
                    nextWords.push_back(words[j]);
                }
            }

            // If we are not at the end of the string of text, add the next
            if (i + window_size < words.size())
            {
                nextWords.push_back(words[i + window_size]);
            }

            // Now update the model using each of the positive context vectors
            // Prev words
            for (std::string cPosWord : prevWords)
            {
                // if (wordVecs[currWord].size() == 0 || contextVecs[cPosWord].size() == 0)
                // {
                //     breaking = true;
                //     break;
                // }
                updateVectors(wordVecs[currWord], contextVecs[cPosWord]);
            }

            // Next words
            for (std::string cPosWord : nextWords)
            {
                // if (wordVecs[currWord].size() == 0 || contextVecs[cPosWord].size() == 0)
                // {
                //     breaking = true;
                //     break;
                // }
                updateVectors(wordVecs[currWord], contextVecs[cPosWord]);
            }
        }

        // Save state every 5k lines trained
        if (lineCount % 5000 == 0)
        {
            std::cout << "Training: line " << lineCount << '\n';
            writeContextVecsToFile(cVecOutput);
            writeWordVecsToFile(wVecOutput);
        }
        lineCount++;
    }
}