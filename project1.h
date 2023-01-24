// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A

#include <vector>
#include <iostream>
#include <deque>
#include <cstdint>
#include <stack>
#include <getopt.h>

using namespace std;

struct coordinate {
    uint16_t level, row, column;
    coordinate(uint16_t l, uint16_t r, uint16_t c) : level(l), row(r), column(c) {}
    coordinate() : level(0), row(0), column(0) {}
};

struct square {
    bool isDiscovered = false;
    char type;
    char direction;
    square(char t) : type(t) {}
    square() : type('u'), direction('u') {}
};

struct path {
    uint16_t l, r, c;
    char direction;
    path(uint16_t le, uint16_t ro, uint16_t co, char d) :
    l(le), r(ro), c(co), direction(d) {}
};

class spaceStation {
private:
    vector<vector<vector<square> > > layout;
    bool inputModeMap;
    bool searchSelected = false;
    bool searchModeStack;
    bool outputModeMap = true;
    bool hangarFound = false;
    uint16_t numFloors;
    uint16_t floorSize;
    coordinate start;
    coordinate hangar;
    coordinate current;
    deque<coordinate> searchContainer;
    stack<path> listOutput;

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
                    if(input[i] == '.' || input[i] == '#' || input[i] == 'E' ) {
                        s.type = input[i];
                    }
                    else if(input[i] == 'H') {
                        s.type = 'H';
                        hangar.level = static_cast<uint16_t>(layout.size());
                        hangar.row = static_cast<uint16_t>(level.size());
                        hangar.column = static_cast<uint16_t>(row.size());
                    }
                    else if(input[i] == 'S') {
                        s.type = 'S';
                        start.level = static_cast<uint16_t>(layout.size());
                        start.row = static_cast<uint16_t>(level.size());
                        start.column = static_cast<uint16_t>(row.size());
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
            square floorSquare('.');
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
                    if(input == '.' || input == '#' || input == 'E') {
                        s.type = input;
                    }
                    else if(input == 'H') {
                        s.type = 'H';
                        hangar.level = level;
                        hangar.row = row;
                        hangar.column = column;
                    }
                    else if(input == 'S') {
                        s.type = 'S';
                        start.level = level;
                        start.row = row;
                        start.column = column;
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
            cout << "Start in level " << start.level << ", row " << 
            start.row << ", column " << start.column << '\n';
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
            if(!hangarFound) {
                return;
            }
            else {
                size_t size = listOutput.size();
                for(size_t i = 0; i < size; i++) {
                    cout << '(' << listOutput.top().l << ',' << listOutput.top().r << ',' 
                    << listOutput.top().c << ',' << listOutput.top().direction << ")\n";
                    listOutput.pop();
                }
            }
        } // output mode for list
    }
    
    // TODO
    void findSolution() {
        searchContainer.push_back(start);
        layout[start.level][start.row][start.column].isDiscovered = true;
        while(!searchContainer.empty()) {
            if(searchModeStack) {
                current = searchContainer.front();
                searchContainer.pop_front();
            }
            else {
                current = searchContainer.back();
                searchContainer.pop_back();
            }

            uint16_t l = current.level;
            uint16_t r = current.row;
            uint16_t c = current.column;

            // check north
            if(r > 0 && layout[l][r - 1][c].isDiscovered == false 
                            && layout[l][r - 1][c].type != '#') {
                if(layout[l][r - 1][c].type == 'H') {
                    hangarFound = true;
                    layout[l][r - 1][c].isDiscovered = true;
                    layout[l][r - 1][c].direction = 'n';
                    searchContainer.clear();
                    return;
                }
                coordinate coor(l, static_cast<uint16_t>(r-1), c);
                layout[l][r - 1][c].isDiscovered = true;
                layout[l][r - 1][c].direction = 'n';
                searchContainer.push_front(coor);
            }
            // check east
            if(c < floorSize - 1 && layout[l][r][c + 1].isDiscovered == false 
                                        && layout[l][r][c + 1].type != '#') {
                if(layout[l][r][c + 1].type == 'H') {
                    hangarFound = true;
                    layout[l][r][c + 1].isDiscovered = true;
                    layout[l][r][c + 1].direction = 'e';
                    searchContainer.clear();
                    return;
                }
                coordinate coor(l, r, static_cast<uint16_t>(c + 1));
                layout[l][r][c + 1].isDiscovered = true;
                layout[l][r][c + 1].direction = 'e';
                searchContainer.push_front(coor);
            }
            //check south
            if(r < floorSize - 1 && layout[l][r + 1][c].isDiscovered == false 
                                        && layout[l][r + 1][c].type != '#') {
                if(layout[l][r + 1][c].type == 'H') {
                    hangarFound = true;
                    layout[l][r + 1][c].isDiscovered = true;
                    layout[l][r + 1][c].direction = 's';
                    searchContainer.clear();
                    return;
                }
                coordinate coor(l, static_cast<uint16_t>(r + 1), c);
                layout[l][r + 1][c].isDiscovered = true;
                layout[l][r + 1][c].direction = 's';
                searchContainer.push_front(coor);
            }
            //check west
            if(c > 0 && layout[l][r][c - 1].isDiscovered == false 
                            && layout[l][r][c - 1].type != '#') {
                if(layout[l][r][c - 1].type == 'H') {
                    hangarFound = true;
                    layout[l][r][c - 1].isDiscovered = true;
                    layout[l][r][c - 1].direction = 'w';
                    searchContainer.clear();
                    return;
                }
                coordinate coor(l, r, static_cast<uint16_t>(c - 1));
                layout[l][r][c - 1].isDiscovered = true;
                layout[l][r][c - 1].direction = 'w';
                searchContainer.push_front(coor);
            }
            // check elevator
            if(layout[l][r][c].type == 'E') {
                for(uint16_t i = 0; i < numFloors; i++) {
                    if(layout[i][r][c].type == 'E' && 
                    layout[i][r][c].isDiscovered == false) {
                        coordinate coor(i, r, c);
                        searchContainer.push_front(coor);
                        layout[i][r][c].isDiscovered = true;
                        layout[i][r][c].direction = static_cast<char>('0' + l);
                    }
                }
            }
        } // while
    } // findSolution

    // TODO
    void tracePath() {
        coordinate loc(hangar.level, hangar.row, hangar.column);
        if(!hangarFound) {
            return;
        }
        // trace back on the layout vector
        if(outputModeMap) {
            while(true) {
                if(layout[loc.level][loc.row][loc.column].direction == 'n') {
                    if(layout[loc.level][loc.row + 1][loc.column].type == 'S') {
                        layout[loc.level][loc.row + 1][loc.column].type = 'n';
                        return;
                    }
                    layout[loc.level][loc.row + 1][loc.column].type = 'n';
                    loc.row++;
                }
                else if(layout[loc.level][loc.row][loc.column].direction == 'e') {
                    if(layout[loc.level][loc.row][loc.column - 1].type == 'S') {
                        layout[loc.level][loc.row][loc.column - 1].type = 'e';
                        return;
                    }
                    layout[loc.level][loc.row][loc.column - 1].type = 'e';
                    loc.column--;
                }
                else if(layout[loc.level][loc.row][loc.column].direction == 's') {
                    if(layout[loc.level][loc.row - 1][loc.column].type == 'S') {
                        layout[loc.level][loc.row - 1][loc.column].type = 's';
                        return;
                    }
                    layout[loc.level][loc.row - 1][loc.column].type = 's';
                    loc.row--;
                }
                else if(layout[loc.level][loc.row][loc.column].direction == 'w') {
                    if(layout[loc.level][loc.row][loc.column + 1].type == 'S') {
                        layout[loc.level][loc.row][loc.column + 1].type = 'w';
                        return;
                    }
                    layout[loc.level][loc.row][loc.column + 1].type = 'w';
                    loc.column++;
                }
                else {
                    // level of the elevator you took to get on this level
                    char levelElevator = layout[loc.level][loc.row][loc.column].direction;
                    // update that elevator to have the level you get off on
                    layout[static_cast<uint16_t>(levelElevator - '0')][loc.row][loc.column].type = static_cast<char>(loc.level + '0');
                    loc.level = static_cast<uint16_t>(levelElevator - '0');
                }
            }
        }
        // trace back using the path stack
        else {
            while(layout[loc.level][loc.row][loc.column].type != 'S') {
                if(layout[loc.level][loc.row][loc.column].direction == 'n') {
                    path p(loc.level, static_cast<uint16_t>(loc.row + 1), loc.column, 'n');
                    loc.row++;
                    listOutput.push(p);
                }
                else if(layout[loc.level][loc.row][loc.column].direction == 'e') {
                    path p(loc.level, loc.row, static_cast<uint16_t>(loc.column - 1), 'e');
                    loc.column--;
                    listOutput.push(p);
                }
                else if(layout[loc.level][loc.row][loc.column].direction == 's') {
                    path p(loc.level, static_cast<uint16_t>(loc.row - 1), loc.column, 's');
                    loc.row--;
                    listOutput.push(p);
                }
                else if(layout[loc.level][loc.row][loc.column].direction == 'w') {
                    path p(loc.level, loc.row, static_cast<uint16_t>(loc.column + 1), 'w');
                    loc.column++;
                    listOutput.push(p);
                }
                else {
                    // level of the elevator you took to get on this level
                    char levelElevator = layout[loc.level][loc.row][loc.column].direction;
                    // update that elevator to have the level you get off on
                    path p(static_cast<uint16_t>(levelElevator - '0'), loc.row, loc.column, static_cast<char>(loc.level + '0'));
                    loc.level = static_cast<uint16_t>(levelElevator - '0');
                    listOutput.push(p);
                }
            }
            
        }
    } // tracePath

}; // spacestation class