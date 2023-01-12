// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <vector>
#include <iostream>
#include <deque>
#include <getopt.h>

using namespace std;


class SpaceStation {
private:
    vector<vector<vector<char> > > layout;
    char inputMode;
    char searchMode = 'a';
    char outputMode = 'M';


    void printHelp() {
        cout << "Usage: ./project1 [-s|-q] [-o M|L] | -h\n";
        cout << "You must specify either stack or queue mode, and if you include the ";
        cout << "output mode then you must specify either M (map) or coordinate list (L)";
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

        while((choice = getopt_long(argc, argv, "sqo:h", long_options, &index))) {
            switch(choice) {
                case 's':
                    if(searchMode != 'a') {
                        
                    }
                    searchMode = 's';
                
                case 'q':
                    searchMode = 'q';
                
                case 'o': {
                    string arg{optarg};
                    if(arg == "M") {
                        outputMode = 'M';
                    }
                    else if (arg == "L") {
                        outputMode = 'L';
                    }
                } // case 'o'
                case 'h':
                    printHelp();
                    exit(0);

                default:
                    cerr << "Error: invalid option\n";
                    exit(1);
            } // switch choice
        } // while choice
    } // getMode

}; // spacestation class