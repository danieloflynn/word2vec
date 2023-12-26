#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"
#include "Word2Vec.h"

int main()
{

    WordFreq wf("wiki.txt");
    wf.countFreqFromFile();
    wf.serialize("dictionary.txt");

    std::cout << "Finished getting freq dictionary" << '\n';
    Word2Vec wv("dictionary.txt");
    wv.makeRandomVecs();
    wv.makeUnigramFreqs();
    wv.train("wiki.txt", "cVecs.txt", "wVecs.txt");
    wv.writeContextVecsToFile("cVecs.txt");
    wv.writeWordVecsToFile("wVecs.txt");
    // for (int i = 0; i < 1000; i++)
    // {

    //     std::string word = wv.getRandomWord();
    //     std::vector<double> vec = wv.contextVecs[word];
    //     std::vector<double> v2 = wv.wordVecs[word];
    //     std::cout << word << " " << vec.size() << " " << v2.size() << "\n";
    // }

    // for (int i = 0; i < 1000; i++)
    // {
    //     std::cout << wv.getRandomWord() << '\n';
    // }

    // for (auto wvec : wv.wordVecs)
    // {
    //     std::cout << wvec.first << " " << wvec.second.size();

    //     std::cout << '\n';
    //     i++;
    //     if (i > 100)
    //     {
    //         break;
    //     }
    // }

    // wv.makeUnigramFreqs();
    // i = 0;
    // for (auto wvec : wv.unigram_freqs)
    // {
    //     std::cout << wvec.first << " ";

    //     std::cout << wvec.first << " " << wvec.second << '\n';

    //     i++;
    //     if (i > 10)
    //     {
    //         break;
    //     }
    // }

    return 0;
}