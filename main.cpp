#include <vector>
#include <iostream>
#include <fstream>

#include "WordFreq.h"

int main()
{
    WordFreq wf("sample.txt");
    wf.countWordFreqs();

    
    return 0;
}