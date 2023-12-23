#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"

int main()
{
    WordFreq wf("wiki.txt");
    wf.countWordFreqs();
    wf.sortWordFreqs();
    int i = 0;
    for (auto &it : wf.sortedWordFreq)
    {
        std::cout << it.first << " " << it.second << '\n';

        i++;
        if (i > 100)
        {
            break;
        }
    }

    wf.serialize("dictionary.txt");

    return 0;
}