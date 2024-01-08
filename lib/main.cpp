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

    Word2Vec wv("./lib/files/dictionary.txt", "wordFreq", 3000);

    wv.readContextVecsFromFile("./lib/files/cVecs2.txt");
    wv.readWordVecsFromFile("./lib/files/wVecs2.txt");
    wv.filterNonNouns("./lib/files/nouns.txt");
    std::cout << wv.contextVecs.size();
    for (int i = 71; i < 80; i++)
    {

        std::ofstream newFile("out" + std::to_string(i) + ".json");
        std::vector<std::pair<std::string, double>> v = wv.calcSimilarWords(wv.dictionary[i]);
        newFile << wv.wordSimToJson(wv.dictionary[i], v);
    }

    // wv.makeRandomVecs();
    // wv.makeUnigramFreqs();
    // wv.train("wiki.txt", "cVecs.txt", "wVecs.txt");
    // wv.writeContextVecsToFile("cVecs.txt");
    // wv.writeWordVecsToFile("wVecs.txt");

    return 0;
}