#include <iostream>
#include <vector>
using namespace std;

void initializeGrid(vector<vector<class Grid>>&);
void Map1(vector<vector<class Grid>>&);
void Map2(vector<vector<class Grid>>&);

const int ROWS = 5;
const int COLS = 5;

enum squareType {
    EMPTY,
    MONEY,
    STINK,
    ENTRY,
    WUMPUS
};

class Grid {
public:
    squareType type;
    int cost;
    string name;
};

string typeToString(squareType t) {
    switch (t) {
    case EMPTY: return "EMPTY";
    case MONEY: return "MONEY";
    case STINK: return "STINK";
    case ENTRY: return "ENTRY";
    case WUMPUS: return "WUMPUS";
    default: return "UNKNOWN";
    }
}

void initializeGrid(vector<vector<Grid>>& GameBoard) {
    GameBoard.resize(ROWS, vector<Grid>(COLS));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            GameBoard[i][j].type = EMPTY;
            GameBoard[i][j].cost = 0;
        }
    }
    Map1(GameBoard);
    //Map2(GameBoard); // Uncomment to use Map2
}

void Map1(vector<vector<Grid>>& GameBoard) {
    GameBoard[4][4].type = MONEY;
    GameBoard[4][4].cost = 1800;

    GameBoard[0][0].type = ENTRY;

    GameBoard[4][1].type = STINK;
    GameBoard[4][3].type = STINK;
    GameBoard[3][2].type = STINK;
    // Removed out-of-bounds [5][2]

    GameBoard[4][2].type = WUMPUS;
    GameBoard[4][2].cost = -10000;
}

void Map2(vector<vector<Grid>>& GameBoard) {
    GameBoard[4][4].type = MONEY;
    GameBoard[4][4].cost = 1700;

    GameBoard[0][2].type = MONEY;
    GameBoard[0][2].cost = 20;

    GameBoard[0][3].type = MONEY;
    GameBoard[0][3].cost = -5;

    GameBoard[1][4].type = MONEY; // Fixed out-of-bounds
    GameBoard[1][4].cost = 10;

    GameBoard[4][1].type = MONEY; // Fixed out-of-bounds
    GameBoard[4][1].cost = -20;

    GameBoard[0][0].type = ENTRY;

    GameBoard[4][1].type = STINK;
    GameBoard[4][3].type = STINK;
    GameBoard[3][2].type = STINK;
    // Removed out-of-bounds [5][2]

    GameBoard[4][2].type = WUMPUS;
    GameBoard[4][2].cost = -10000;
}

int main() {
    vector<vector<Grid>> GameBoard;
    initializeGrid(GameBoard);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << "Grid[" << i << "][" << j << "] - Type: " << typeToString(GameBoard[i][j].type)
                << ", Cost: " << GameBoard[i][j].cost << endl;
        }
    }
    cout << "Wumpus World Initialized" << endl;
    return 0;
}
