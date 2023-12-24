#ifndef WORD2VEC_H
#define WORD2VEC_H

#include <string>
#include <vector>
#include <unordered_map>
#include <random>

class Word2Vec
{
public:
    Word2Vec(std::string fileName, std::string parseType = "wordFreq", int minCount = 1, int dimension = 64);
    int dimension;
    const double vec_lower_bound = -10000.0;
    const double vec_upper_bound = 10000.0;
    const double learning_rate = 0.1;        // Set this to 0.1 per book
    const int window_size = 2;               // number of words to look at either side of the current word
    const int ratio_neg_context_vectors = 2; // Ratio of negative to positive context vectors
    static std::random_device rd;
    std::vector<std::string> dictionary;
    std::vector<std::pair<std::string, int>> wordFreqs;
    std::unordered_map<std::string, std::vector<double>> wordVecs;
    std::unordered_map<std::string, std::vector<double>> contextVecs;
    std::vector<std::pair<std::string, double>> unigram_freqs;
    std::discrete_distribution<> unigram_dist;
    void makeRandomVecs();
    void writeVecsToFile();
    void readContextVecsFromFile();
    void readWordVecsFromFile(std::string fileName);
    void getTotalWordCount(std::string fileName);
    void makeUnigramFreqs(double alpha = 0.75);
    void writeUnigramFreqsToFile(std::string fileName);
    void readUnigramFreqsFromFile(std::string fileName);
    std::string getRandomWord();
    std::vector<double> scalarMult(std::vector<double> &vec, double scalar);
    std::vector<double> vectorAdd(std::vector<double> &vec1, std::vector<double> &vec2);
    double dotProd(std::vector<double> &vec1, std::vector<double> &vec2);
    double sigmoid(double num);
    void updateCPosVec(std::vector<double> &cPosVec, std::vector<double> &wVec);
    void updateCNegVec(std::vector<double> &cNegVec, std::vector<double> &wVec);
    void updateWVec(std::vector<double> &wVec, std::vector<double> &cPosVec, std::vector<std::vector<double> *> &cNegVecs);
    void updateVectors(std::vector<double> &wVec, std::vector<double> &cPosVec);
    void train(std::string trainingText);
    void serialize(std::string outputFile);
};

#endif