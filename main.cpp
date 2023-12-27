#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"
#include "Word2Vec.h"

int main()
{

    // WordFreq wf("wiki.txt");
    // wf.countFreqFromFile();
    // wf.serialize("dictionary.txt");
    // std::cout << "Finished getting freq dictionary" << '\n';

    Word2Vec wv("dictionary.txt", "wordFreq", 500);

    // wv.readContextVecsFromFile("cVecs.txt");
    // wv.readWordVecsFromFile("wVecs.txt");

    // int i = 100;

    // for (int i = 100; i < 101; i++)
    // {
    //     std::vector<std::pair<std::string, double>> v = wv.calcSimilarWords(wv.dictionary[i]);
    //     for (int j = 0; j < 100; j++)
    //     {
    //         std::cout << wv.dictionary[i] << " " << v[j].first << " " << v[j].second << '\n';
    //     }
    // }

    wv.makeRandomVecs();
    wv.makeUnigramFreqs();
    wv.train("wiki.txt", "cVecs.txt", "wVecs.txt");
    // wv.writeContextVecsToFile("cVecs.txt");
    // wv.writeWordVecsToFile("wVecs.txt");

    return 0;
}