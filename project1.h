// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <vector>
#include <iostream>
#include <deque>
#include <cstdint>
#include <getopt.h>

using namespace std;

struct coordinate {
    uint16_t level, row, column;
};

struct square {
    bool isDiscovered = false;
    char type;
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
    coordinate startPosition;
    coordinate currentLocation;
    deque<coordinate> searchContainer;

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
    } // inputLayoutSettings

    // EFFECTS reads from cin to get the layout of the inputted space station
    void inputLayoutTiles() {
        // read input in map format
        if(inputModeMap) {
            string input;
            layout.reserve(numFloors);
            vector<vector<square>> level;
            level.reserve(floorSize);
            getline(cin, input);
            while(cin >> input) {
                // check that the line isn't a comment
                if(input[0] == '/') {
                    getline(cin, input);
                    continue;
                }
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
                    if(input[i] == '.' || input[i] == '#' || input[i] == 'E' || 
                                                            input[i] == 'H') {
                        s.type = input[i];
                    }
                    else if(input[i] == 'S') {
                        s.type = 'S';
                        startPosition.level = static_cast<uint16_t>(layout.size());
                        startPosition.row = static_cast<uint16_t>(level.size());
                        startPosition.column = static_cast<uint16_t>(row.size());
                    }
                    else {
                        cerr << "Invalid map character\n";
                        exit(1);
                    }
                    row.push_back(s);
                }
                level.push_back(row);
            } // while getline
            layout.push_back(level);
        } // map mode input
        // read input in list format
        else {
            char input;
            string trash;
            square floorSquare;
            floorSquare.type = '.';
            // fills every spot in the 3d vector with floor tiles
            layout.resize(numFloors, vector<vector<square> >
                (floorSize, vector<square>(floorSize, floorSquare)));
            getline(cin, trash);
            while(cin >> input) {
                uint16_t level;
                uint16_t column;
                uint16_t row;
                if(input == '/') {
                    getline(cin, trash);
                    continue;
                }
                else {
                    square s;
                    // reads in level, comma, column, comma, row, comma, 
                    // then the type of square
                    cin >> level;
                    cin >> input;
                    cin >> row;
                    cin >> input;
                    cin >> column;
                    cin >> input;
                    cin >> input;
                    if(input == '.' || input == '#' || input == 'E' || input == 'H') {
                        s.type = input;
                    }
                    else if(input == 'S') {
                        s.type = 'S';
                        startPosition.level = level;
                        startPosition.row = row;
                        startPosition.column = column;
                    }
                    else {
                        cerr << "Invalid map character\n";
                        exit(1);
                    }
                    if(level >= numFloors) {
                        cerr << "Invalid map level\n";
                        exit(1);
                    }
                    else if(row >= floorSize) {
                        cerr << "Invalid map row\n";
                        exit(1);
                    }
                    else if(column >= floorSize) {
                        cerr << "Invalid map column\n";
                        exit(1);
                    }
                    layout[level][row][column] = s;
                    cin >> input;
                }
            } // while
        } // list mode input
    } // inputLayoutTiles

    void outputSolution() {
        if(outputModeMap) {
            cout << "Start in level " << startPosition.level << ", row " << 
            startPosition.row << ", column " << startPosition.column << '\n';
            for(int i = 0; i < numFloors; i++) {
                cout << "//level " << i << '\n';
                for(int j = 0; j < floorSize; j++) {
                    for(int k = 0; k < floorSize; k++) {
                        cout << layout[i][j][k].type; 
                    }
                    cout << "\n";
                }
            }
        }
        else { //TODO
            cout << "//path taken\n";
        } // output mode for list
    }
    
    // TODO
    void findSolution() {
        searchContainer.push_back(startPosition);
        layout[startPosition.level][startPosition.row][startPosition.column].isDiscovered 
                                                                                = true;
        while(!searchContainer.empty()) {
            currentLocation = searchContainer.front();
            searchContainer.pop_front();

            uint16_t level = currentLocation.level;
            uint16_t row = currentLocation.row;
            uint16_t column = currentLocation.column;
            // check north
            if(row > 0 && layout[level][row - 1][column].isDiscovered == false && 
                                    layout[level][row - 1][column].type != '#') {
                coordinate c;
                c.level = level;
                c.row = row - 1;
                c.column = column;
                if(searchModeStack) {
                    searchContainer.push_front(c);
                }
                else {
                    searchContainer.push_back(c);
                }
            }
            // check east
            if(column < floorSize - 1 && layout[level][row][column + 1].isDiscovered == false && 
                                    layout[level][row + 1][column].type != '#') {
                coordinate c;
                c.level = level;
                c.row = row;
                c.column = column + 1;
                if(searchModeStack) {
                    searchContainer.push_front(c);
                }
                else {
                    searchContainer.push_back(c);
                }
            }
            //check south
            if(row < floorSize - 1 && layout[level][row + 1][column].isDiscovered == false && 
                                    layout[level][row + 1][column].type != '#') {
                coordinate c;
                c.level = level;
                c.row = row + 1;
                c.column = column;
                if(searchModeStack) {
                    searchContainer.push_front(c);
                }
                else {
                    searchContainer.push_back(c);
                }
            }
            //check west
            if(column > 0 && layout[level][row][column - 1].isDiscovered == false && 
                                    layout[level][column - 1][column].type != '#') {
                coordinate c;
                c.level = level;
                c.row = row;
                c.column = column - 1;
                if(searchModeStack) {
                    searchContainer.push_front(c);
                }
                else {
                    searchContainer.push_back(c);
                }
            }
        } // while
    }
}; // spacestation class