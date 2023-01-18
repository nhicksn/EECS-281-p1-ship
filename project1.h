// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <vector>
#include <iostream>
#include <deque>
#include <cstdint>
#include <getopt.h>

using namespace std;

enum squareType : uint8_t {
    wall,
    elevator,
    hangar,
    floor,
    start
};

struct square {
    bool isDiscovered = false;
    squareType type;
};

class spaceStation {
private:
    vector<vector<vector<square> > > layout;
    bool inputModeMap;
    bool searchSelected = false;
    bool searchModeStack;
    bool outputModeMap = true;
    uint16_t numFloors;
    uint16_t floorSize;


    void printHelp() {
        cout << "Usage: ./ship [-s|-q] [-o M|L] | -h\nYou must specify either stack ";
        cout << "or queue mode, and if you include the output mode ";
        cout << "then you must specify either map (M) or coordinate list (L)\n";
    } // printHelp

public:

    // custom constructor
    // EFFECTS takes in the command line arguments and retrieves the settings
    spaceStation(const int &argc, char *argv[]) {
        getMode(argc, argv);
        inputLayoutSettings();
    }

    // EFFECTS This function processes the command line arguments and gets the mode that
    // the algorithm should be running in
    void getMode(const int &argc, char *argv[]) {
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
                    if(searchSelected == true) {
                        cerr << "Multiple routing modes specified\n";
                        exit(1);
                    } // if
                    searchModeStack = true;
                    searchSelected = true;
                    break;
                } // case 's'
                
                case 'q': {
                    if(searchSelected == true) {
                        cerr << "Multiple routing modes specified\n";
                        exit(1);
                    } // if
                    searchModeStack = false;
                    searchSelected = true;
                    break;
                } // case 'q'
                
                case 'o': {
                    string arg{optarg};
                    if(arg == "M") {
                        outputModeMap = true;
                    } // if
                    else if (arg == "L") {
                        outputModeMap = false;
                    } // if
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
        if(searchSelected == false) {
            cerr << "No routing mode specified\n";
            exit(1);
        } // if
    } // getMode

    // EFFECTS Returns true if the sorting mode is stack, and false if it is queue
    bool stackMode() {
        if(searchModeStack == true) return true;
        return false;
    }

    // EFFECTS Returns true if the output mode is map, and false if it is coordinate list
    bool mapMode() {
        if(outputModeMap == true) return true;
        return false;
    }

    // EFFECTS reads from cin to find the settings of the input file
    void inputLayoutSettings() {
        string input;
        cin >> input;
        if(input == "M") {
            inputModeMap = true;
        }
        else inputModeMap = false;
        cin >> numFloors;
        cin >> floorSize;

        // included in inputLayout now
        // if(inputModeMap) {
        //     // need to push back each floor to this vector
        //     // create 2d vectors of each floor and push back to it
        //     layout.reserve(numFloors);
        // }
        // else {
        //     square floorSquare;
        //     floorSquare.type = floor;
        //     // fills every spot in the 3d vector with floor tiles
        //     layout.resize(numFloors, vector<vector<square> >(floorSize, vector<square>(floorSize, floorSquare)));
        // }
    } // inputLayoutSettings

    // EFFECTS reads from cin to get the layout of the inputted space station
    void inputLayoutTiles() {
        string input;
        // read input in map format
        if(inputModeMap) {
            layout.reserve(numFloors);
            vector<vector<square>> level;
            level.reserve(floorSize);
            getline(cin, input);
            while(getline(cin, input)) {
                // check that the line isn't a comment
                if(input[0] == '/' && input[1] == '/') continue;
                // initialize vector to store this line
                vector<square> row;
                row.reserve(floorSize);
                // check that this level isn't full
                if(level.size() == floorSize) {
                    // if it is, push back to layout and move on to next level
                    layout.push_back(level);
                    level.clear();
                }
                for(size_t i = 0; i < input.size(); i++) {
                    square s;
                    if(input[i] == '.') {
                        s.type = floor;
                    }
                    else if(input[i] == '#') {
                        s.type = wall;
                    }
                    else if(input[i] == 'E') {
                        s.type = elevator;
                    }
                    else if(input[i] == 'H') {
                        s.type = hangar;
                    }
                    else {
                        s.type = start;
                    }
                    row.push_back(s);
                }
                level.push_back(row);
            } // while getline
            layout.push_back(level);
        } // map mode input
        // read input in list format
        else {
            square floorSquare;
            floorSquare.type = floor;
            // fills every spot in the 3d vector with floor tiles
            layout.resize(numFloors, vector<vector<square> >
                (floorSize, vector<square>(floorSize, floorSquare)));
            getline(cin, input);
            while(getline(cin, input)) {
                // check that the input isn't a comment
                if(input[0] == '/' && input[1] == '/') continue;
                square s;
                if(input[7] == '.') {
                    s.type = floor;
                }
                else if(input[7] == '#') {
                    s.type = wall;
                }
                else if(input[7] == 'E') {
                    s.type = elevator;
                }
                else if(input[7] == 'H') {
                    s.type = hangar;
                }
                else {
                    s.type = start;
                }
                // segfaults here
                layout[input[1]][input[3]][input[5]] = s;
            } // while getline
        } // list mode input
    } // inputLayoutTiles

    void outputLayout() {
        for(int i = 0; i < numFloors; i++) {
            for(int j = 0; j < floorSize; j++) {
                for(int k = 0; k < floorSize; k++) {
                    cout << layout[i][j][k].type; 
                }
                cout << "\n";
            }
            cout << "\n";
        }
    }
}; // spacestation class


class searchContainer {
public:
    deque<square> search;

private:
    void searchSquare() {
        return;
    } 

}; // searchContainer class