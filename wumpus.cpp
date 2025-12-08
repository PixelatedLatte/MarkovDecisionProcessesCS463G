#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <array>
#include <tuple>
#include<cmath>

using namespace std;
void initializeGrid(vector<vector<class Grid>>&);
void Map1(vector<vector<class Grid>>&);
void Map2(vector<vector<class Grid>>&);

const int ROWS = 6;
const int COLS = 6;
const double DISCOUNT_FACTOR = 0.95;

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
    case EMPTY: return "EM";
    case MONEY: return "M";
    case STINK: return "S";
    case ENTRY: return "EN";
    case WUMPUS: return "W";

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
    GameBoard[5][2].type = STINK;

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

    GameBoard[1][4].type = MONEY;
    GameBoard[1][4].cost = 10;

    GameBoard[4][1].type = MONEY;
    GameBoard[4][1].cost = -20;

    GameBoard[0][0].type = ENTRY;

    GameBoard[4][1].type = STINK;
    GameBoard[4][3].type = STINK;
    GameBoard[3][2].type = STINK;

    GameBoard[4][2].type = WUMPUS;
    GameBoard[4][2].cost = -10000;
}

void policyIteration(vector<vector<Grid>>& GameBoard, double valueMatrix[ROWS][COLS]) {
    // Placeholder for policy iteration implementation
}

string movement(string attemptedMove) {
    array<string, 3> moveset;
    array<double, 3> moveProbabilities = { 0.7, 0.15, 0.15 }; // forward, reverse, stall

    if (attemptedMove == "^") {
        moveset = { "^", "v", "STALL" };
    }
    else if (attemptedMove == "v") {
        moveset = { "v", "^", "STALL" };
    }
    else if (attemptedMove == "<") {
        moveset = { "<", ">", "STALL" };
    }
    else if (attemptedMove == ">") {
        moveset = { ">", "<", "STALL" };
    }
    else {
        return "INVALID";
    }

    double probability = (double)rand() / RAND_MAX; // random number 0→1
    double cumulative = 0.0;

    for (int j = 0; j < 3; j++) {
        cumulative += moveProbabilities[j];
        if (probability <= cumulative) {
            return moveset[j];
        }
    }
    return "INVALID";
}

tuple<int, int> applymovement(string appliedMove, vector<vector<Grid>>& GameBoard, int x, int y) {
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) {
        cout << "Out of bounds position!" << endl;
        return make_tuple(0, 0);
    }

    if (appliedMove == "STALL") {
        return make_tuple(x, y);
    }
    else if (appliedMove == "^") {       // GO UP (increase y)
        y = min(ROWS - 1, y + 1);
    }
    else if (appliedMove == "v") {       // GO DOWN (decrease y)
        y = max(0, y - 1);
    }
    else if (appliedMove == "<") {       // GO LEFT (decrease x)
        x = max(0, x - 1);
    }
    else if (appliedMove == ">") {       // GO RIGHT (increase x)
        x = min(COLS - 1, x + 1);
    }
    return make_tuple(x, y);
}

void printGrid(vector<vector<Grid>>& GameBoard, int xpos, int ypos) {
    for (int row = ROWS - 1; row >= 0; row--) {   // y
        for (int col = 0; col < COLS; col++) {    // x
            const string cellType = typeToString(GameBoard[col][row].type);
            // Use consistent width for all cells; when player is here, show P and the type
            if (col == xpos && row == ypos) {
                printf("[P %2s]  ", cellType.c_str());
            }
            else {
                // regular cell - 4 spaces inside brackets to align with [P xx]
                printf("[%4s]  ", cellType.c_str());
            }
        }
        cout << endl;
    }
    cout << endl;
}

vector<vector<string>> policyInitialization(vector<vector<Grid>>& GameBoard) {
    vector<vector<string>> policyGrid(ROWS, vector<string>(COLS));
    array<string, 4> policy = { "^", "v", "<", ">" };
    for (int row = ROWS - 1; row >= 0; row--) {   // y
        for (int col = 0; col < COLS; col++) {    // x
            policyGrid[col][row] = policy[rand() % 4];
            cout << "[ " << policyGrid[col][row] << " ] ";
        }
        cout << endl;
    }
    return policyGrid;
}

int policyEvaluation(vector<vector<Grid>>& GameBoard, vector<vector<string>>& policyGrid, int xpos, int ypos, double gamma, int timeHorizon, int step, int& totalvalue) {
    if (timeHorizon == 0) {
        cout << "Total Value Collected: " << totalvalue << endl;
        return totalvalue;
    }
    string appliedMovement = movement(policyGrid[xpos][ypos]);
    // FIXED: Changed from policyGrid[ypos][xpos] to policyGrid[xpos][ypos]
    cout << "Attempted move: " << policyGrid[xpos][ypos] << ", Applied move: " << appliedMovement << endl;
    tie(xpos, ypos) = applymovement(appliedMovement, GameBoard, xpos, ypos);
    double discount = pow(gamma, step);
    totalvalue += GameBoard[xpos][ypos].cost * discount;
    cout << "Step: " << step << ", New Position: (" << xpos << ", " << ypos << "), Current Total Value: " << totalvalue << endl;
    printGrid(GameBoard, xpos, ypos);
    policyEvaluation(GameBoard, policyGrid, xpos, ypos, gamma, timeHorizon - 1, step + 1, totalvalue);
    return totalvalue;
}

int main() {
    double valueMatrix[ROWS][COLS] = {0.0};
    int xpos, ypos, timeHorizon, totalValue;
    xpos = 0;
    ypos = 0;
    double gamma = .95;
    totalValue = 0;
    timeHorizon = 100;
    vector<vector<Grid>> GameBoard;
    initializeGrid(GameBoard);
    srand(time(NULL));

    printGrid(GameBoard, xpos, ypos);
    cout << "Wumpus World Initialized! !" << endl;
    string appliedMove = "";

    vector<vector<string>> policyGrid = policyInitialization(GameBoard);
    int endValue = policyEvaluation(GameBoard, policyGrid, xpos, ypos, gamma, timeHorizon, 0, totalValue);
    cout << "End Value from Policy Evaluation: " << endValue << endl;

    return 0;
}