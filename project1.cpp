// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <iostream>
#include <vector>
#include <deque>
#include <getopt.h>
#include <cstdint>
#include "project1.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    spaceStation s(argc, argv);
    s.inputLayoutSettings();
    s.outputMap();
    s.inputLayoutTiles();
} // main

// resize for coordinate list, with "." being the default char
// reserve for map, populating it with the input