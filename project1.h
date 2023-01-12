// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <vector>
#include <iostream>
#include <deque>
#include <getopt.h>

using namespace std;


class spaceStation {
private:
    vector<vector<vector<char> > > layout;
    char inputMode;
    char searchMode = 'a';
    char outputMode = 'M';


    void printHelp() {
        cout << "Usage: ./ship [-s|-q] [-o M|L] | -h\nYou must specify either stack ";
        cout << "or queue mode, and if you include the output mode ";
        cout << "then you must specify either map (M) or coordinate list (L)\n";
    } // printHelp

public:
    void getMode(int argc, char *argv[]) {
        opterr = false;
        int choice;
        int index = 0;
        option long_options[] = {
            { "stack",  no_argument,       nullptr, 's' },
            { "queue",  no_argument,       nullptr, 'q' },
            { "output", required_argument, nullptr, 'o' },
            { "help",   no_argument,       nullptr, 'h' },
            { nullptr,  0,                 nullptr, '\0'},
        }; // long_options

        while((choice = getopt_long(argc, argv, "sqo:h", long_options, &index)) != -1) {
            switch(choice) {
                case 's': {
                    if(searchMode != 'a') {
                        cerr << "Multiple routing modes specified\n";
                        exit(1);
                    }
                    searchMode = 's';
                    break;
                } // case 's'
                
                case 'q': {
                    if(searchMode != 'a') {
                        cerr << "Multiple routing modes specified\n";
                        exit(1);
                    }
                    searchMode = 'q';
                    break;
                } // case 'q'
                
                case 'o': {
                    string arg{optarg};
                    if(arg == "M") {
                        outputMode = 'M';
                    }
                    else if (arg == "L") {
                        outputMode = 'L';
                    }
                    break;
                } // case 'o'
                case 'h': {
                    printHelp();
                    exit(0);
                    break;
                } // case 'h'

                default: {
                    cerr << "Error: invalid option\n";
                    exit(1);
                } // default case
            } // switch choice
        } // while choice
        if(searchMode == 'a') {
            cerr << "No routing mode specified\n";
            exit(1);
        }
    } // getMode
}; // spacestation class