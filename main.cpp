#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"

int main()
{
    WordFreq wf("sample.txt");
    wf.countWordFreqs();
    wf.sortWordFreqs();
    for (auto &it : wf.sortedWordFreq)
    {
        std::cout << it.first << " " << it.second << '\n';
    }

    return 0;
}