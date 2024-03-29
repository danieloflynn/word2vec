#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <unordered_set>

#include "ThreadPool.h"
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
// TODO: allow "-"
inline int Word2Vec::isPunctuation(char &text)
{
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
Makes random initial word and context vectors for each word in the dictionary
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
        double wordTotal = 0;
        double contextTotal = 0;

        // Make word and context mutexes
        wordVecMutexes[word] = std::make_unique<std::mutex>();
        contextVecMutexes[word] = std::make_unique<std::mutex>();

        for (int i = 0; i < dimension; i++)
        {
            double random_double = unif(re);
            wordTotal += random_double * random_double;
            wordVec.push_back(random_double);

            random_double = unif(re);
            contextTotal += random_double * random_double;
            contVec.push_back(random_double);
        }

        // Divide each vector by total to get softmaxed value
        for (int i = 0; i < dimension; i++)
        {
            wordVec[i] /= sqrt(wordTotal);
            contVec[i] /= sqrt(contextTotal);
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

// Writes word vectors to a file.
void Word2Vec::writeWordVecsToFile(std::string fileName)
{
    std::ofstream newFile(fileName);
    for (auto &word : wordVecs)
    {
        // Skip empty word vectors
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

// Add the second vector to the first
void Word2Vec::vectorAdd(std::vector<double> &vec1, std::vector<double> &vec2)
{
    // If vectors are different sizes print error and return null vector
    if (vec1.size() != vec2.size())
    {
        std::cout << "Error: vectors are different sizes." << '\n';
        return;
    }

    for (int i = 0; i < vec1.size(); i++)
    {
        vec1[i] += vec2[i];
    }
}

// Gets the dot product of two vectors
// Per the book this is actually the cosine, not just the dot product
// i.e. we divide by the magnitude of both vectors after performing the dot product
double Word2Vec::dotProd(std::vector<double> &vec1, std::vector<double> &vec2)
{

    if (vec1.size() != vec2.size())
    {
        std::cout << "Error: vectors are different sizes." << '\n';
        return 0.0;
    }

    double total = 0;
    double vec1_len = 0;
    double vec2_len = 0;

    for (int i = 0; i < vec1.size(); i++)
    {
        vec1_len += vec1[i] * vec1[i];
        vec2_len += vec2[i] * vec2[i];
        total += vec1[i] * vec2[i];
    }

    return total / (sqrt(vec1_len) * sqrt(vec2_len));
}

/*
Returns sigmoid of the number.
sigmoid(x) = 1/(1+e^(-x))
*/
double Word2Vec::sigmoid(double num)
{
    return 1 / (1 + exp(-num));
}

// Normalizes a vector input
void Word2Vec::softMax(std::vector<double> &vec)
{
    // Get the magnitude of the vector^2
    double total = 0;
    for (double &v : vec)
    {
        total += v * v;
    }
    // Divide each element in the vector by the magnitude
    for (double &v : vec)
    {
        v /= sqrt(total);
    }
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
        // Skip if both words are the same
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
    std::sort(wordSimilarity.begin(), wordSimilarity.end(), cmp);
    return wordSimilarity;
}

/*
Converts word similarity to JSON string
*/
std::string Word2Vec::wordSimToJson(std::string word, std::vector<std::pair<std::string, double>> &similarWords)
{
    std::string json = "{ \n";
    json += "\"word\" : \"" + word + "\", \n";
    json += "\"similarities\" : [\n";

    for (int i = 0; i < similarWords.size(); i++)
    {
        json += "{\n\"simWord\" : \"" + similarWords[i].first + "\",\n  ";
        json += "\"rank\" : " + std::to_string(i + 1) + ", \n";
        json += "\"similarity\" : \"" + std::to_string(similarWords[i].second) + "\" \n},";
    }
    json.pop_back(); // Remove the last comma
    json += "] \n }";

    return json;
}

/*
Updates the positive context vector.
c_pos(t+1) = c_pos(t) - learn_rate * [sigmoid(c_neg(t)*w(t)) - 1] w(t)
*/
void Word2Vec::updateCPosVec(std::vector<double> &cPosVec, std::vector<double> &wVec)
{

    double scalar = -learning_rate * (sigmoid(dotProd(cPosVec, wVec)) - 1);
    std::vector<double> newVec = scalarMult(wVec, scalar);
    vectorAdd(cPosVec, newVec);
    softMax(cPosVec);
}

/*
Updates the negative context vector.
c_neg(t+1) = c_neg(t) - learn_rate * [sigmoid(c_pos(t)*w(t))] w(t)
*/
void Word2Vec::updateCNegVec(std::vector<double> &cNegVec, std::vector<double> &wVec)
{
    double scalar = -learning_rate * (sigmoid(dotProd(cNegVec, wVec)));
    std::vector<double> newVec = scalarMult(wVec, scalar);
    vectorAdd(cNegVec, newVec);
    softMax(cNegVec);
}

/*
Updates the word vector
w(t+1) = w(t) - learn_rate[ [sigmoid(c_pos(t)*w(t)) - 1] c_pos + sum_all_c_neg(sigmoid(c_neg(t)*w(t)) ) c_neg ]

Do it in place to save time. ith element of w:
w(t+1)[i] = w(t)[i] - learn_rate[ [sigmoid(c_pos(t)*w(t)) - 1] c_pos[i] + sum_all_c_neg(sigmoid(c_neg(t)*w(t)) ) c_neg[i] ]
*/
void Word2Vec::updateWVec(std::vector<double> &wVec, std::vector<double> &cPosVec, std::vector<std::vector<double> *> &cNegVecs)
{

    // Calculate the scalar parts
    double cPosScalar = -learning_rate * (sigmoid(dotProd(cPosVec, wVec)) - 1);

    std::vector<double> cNegScalars;
    for (std::vector<double> *&cNegVec : cNegVecs)
    {
        cNegScalars.push_back(-learning_rate * (sigmoid(dotProd((*cNegVec), wVec))));
    }

    // Iterate over word vector and add cPos and cNeg parts
    for (int i = 0; i < wVec.size(); i++)
    {
        wVec[i] += cPosScalar * cPosVec[i];
        for (int j = 0; j < cNegVecs.size(); j++)
        {
            wVec[i] += cNegScalars[j] * (*cNegVecs[j])[i];
        }
    }
    softMax(wVec);
}

/*
Takes a positive context vector and word vector.
Creates k negative context vector.
Updates the positive, negative and word vectors using the update equations.
*/
void Word2Vec::updateVectors(std::string &wVecWord, std::string &cVecWord, std::vector<double> &wVec, std::vector<double> &cPosVec)
{

    // First, get random negative context vectors.

    std::vector<std::vector<double> *> cNegVecs;
    std::vector<std::string> cNegWords;

    // Get cNeg words and vecs
    for (int i = 0; i < ratio_neg_context_vectors; i++)
    {
        std::string randWord = getRandomWord();
        cNegWords.push_back(randWord);
        cNegVecs.push_back(&contextVecs[randWord]);
    }

    for (int i = 0; i < cNegVecs.size(); i++)
    {
        (*contextVecMutexes[cNegWords[i]]).lock();
        updateCNegVec(*(cNegVecs[i]), wVec);
        (*contextVecMutexes[cNegWords[i]]).unlock();
    }

    // update positive context vectors
    (*contextVecMutexes[cVecWord]).lock();
    updateCPosVec(cPosVec, wVec);
    (*contextVecMutexes[cVecWord]).unlock();

    // Update the word vector
    (*wordVecMutexes[wVecWord]).lock();
    updateWVec(wVec, cPosVec, cNegVecs);
    (*wordVecMutexes[wVecWord]).unlock();
}

/*
Process line of text and learn n-gram
*/

void Word2Vec::processLine(std::string text, int lineCount)
{

    std::vector<std::string> words;
    std::stringstream ss(text);
    if (lineCount % 100 == 0)
    {

        std::cout << "Line " << lineCount << '\n';
    }
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
        return;
    }

    // Now iterate over the sliding window
    for (int i = 0; i < words.size(); i++)
    {
        // If current word not in dictionary, skip
        if (dictSet.find(words[i]) == dictSet.end())
        {
            continue;
        }

        // Update the model using each of the positive context vectors
        // words before current word
        for (int j = std::max(0, i - window_size); j < i; j++)
        {
            // Check that context word exists in dictionary
            if (dictSet.find(words[j]) == dictSet.end())
            {
                continue;
            }

            updateVectors(words[i], words[j], wordVecs[words[i]], contextVecs[words[j]]);
        }

        // words after current word
        for (int j = i + 1; j < std::min(i + window_size + 1, (int)words.size()); j++)
        {
            // Check that context word exists in dictionary
            if (dictSet.find(words[j]) == dictSet.end())
            {
                continue;
            }
            updateVectors(words[i], words[j], wordVecs[words[i]], contextVecs[words[j]]);
        }
    }
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
    double learnTime = 0.0;

    auto start = std::chrono::high_resolution_clock::now();
    auto totalStart = std::chrono::high_resolution_clock::now();

    int maxThreads = std::thread::hardware_concurrency();
    ThreadPool threadpool; // To hold the
    threadpool.Start();

    // Get a line
    while (getline(myFile, text))
    {
        // Run concurrently
        threadpool.QueueJob([&, text, lineCount]()
                            { processLine(text, lineCount); });

        // Save state every 5k lines trained
        if (lineCount % 200000 == 0)
        {

            std::cout << "Waiting for threads to stop" << '\n';
            threadpool.Stop();
            for (std::string word : dictionary)
            {
                (*contextVecMutexes[word]).lock();
                (*wordVecMutexes[word]).lock();
            }

            writeContextVecsToFile(cVecOutput);
            writeWordVecsToFile(wVecOutput);
            for (std::string word : dictionary)
            {
                (*contextVecMutexes[word]).unlock();
                (*wordVecMutexes[word]).unlock();
            }

            auto totalStop = std::chrono::high_resolution_clock::now();
            double totalTime = std::chrono::duration_cast<std::chrono::microseconds>(totalStop - totalStart).count();
            std::cout << "Total time for " << lineCount << " lines: " << totalTime / 1000000 << '\n';

            // Start the threads again
            threadpool.Start();
        }
        lineCount++;

        start = std::chrono::high_resolution_clock::now();
    }
}

/*Filter out non nouns from a list of known nouns*/
void Word2Vec::filterNonNouns(std::string nounFile)
{
    // Read in text
    std::fstream myFile(nounFile);
    std::string text;

    std::unordered_map<std::string, std::vector<double>> newWordVecs;
    std::unordered_map<std::string, std::vector<double>> newContextVecs;
    std::vector<std::string> newDictionary;
    std::unordered_set<std::string> newDictSet;

    while (getline(myFile, text))
    {
        if (dictSet.find(text) != dictSet.end())
        {
            newWordVecs[text] = wordVecs[text];
            newContextVecs[text] = contextVecs[text];
            newDictSet.insert(text);
            newDictionary.push_back(text);
        }
    }

    wordVecs = newWordVecs;
    contextVecs = newContextVecs;
    dictSet = newDictSet;
    dictionary = newDictionary;
}