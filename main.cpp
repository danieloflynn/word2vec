#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"
#include "Word2Vec.h"

int main()
{

    WordFreq wf("wiki.txt");
    wf.countWordFreqs(100000);
    wf.serialize("dictionary.txt");
    // Word2Vec wv("dictionary.txt");
    // wv.makeRandomWordVecs();
    // int i = 0;
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