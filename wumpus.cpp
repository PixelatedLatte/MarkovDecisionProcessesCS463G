#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <array>
#include <tuple>
#include<cmath>
#include <cstdlib>
#include <ctime>

using namespace std;
void initializeGrid(vector<vector<class Grid>>&);
void Map1(vector<vector<class Grid>>&);
void Map2(vector<vector<class Grid>>&);

const int ROWS = 6;
const int COLS = 6;
const double DISCOUNT_FACTOR = 0.95;
const int POLICY_SIZE = 4;
int PolicyCheck = 0;

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
    case EMPTY: return "empty";
    case MONEY: return "MONEY";
    case STINK: return "STINK";
    case ENTRY: return "ENTER";
    case WUMPUS: return "WUMP";

    default: return "UNKNOWN";
    }
}

void initializeGrid(vector<vector<Grid>>& GameBoard) {
    GameBoard.resize(ROWS, vector<Grid>(COLS));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            GameBoard[j][i].type = EMPTY;
            GameBoard[j][i].cost = 0;
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
    GameBoard[5][2].type = STINK;

    GameBoard[4][2].type = WUMPUS;
    GameBoard[4][2].cost = -10000;
}

double nextAction(int r, int c, int action, vector<vector<double>>& valueMatrix, vector<vector<Grid>>& GameBoard)
{
    // Directions match your indexing
    const int dc[4] = { -1,  1,  0,  0 }; // left, right
    const int dr[4] = { 0,  0,  1, -1 }; // up, down  (row increases upward)

    // Reverse action
    int rev = (action % 2 == 0) ? action + 1 : action - 1;

    int nc = c + dc[action];
    int nr = r + dr[action];

    int rc = c + dc[rev];
    int rr = r + dr[rev];

    // Clamp to boundaries
    if (nc < 0 || nc >= COLS || nr < 0 || nr >= ROWS) { nc = c; nr = r; }
    if (rc < 0 || rc >= COLS || rr < 0 || rr >= ROWS) { rc = c; rr = r; }

    double stay = GameBoard[c][r].cost + DISCOUNT_FACTOR * valueMatrix[c][r];
    double move = GameBoard[nc][nr].cost + DISCOUNT_FACTOR * valueMatrix[nc][nr];
    double revMv = GameBoard[rc][rr].cost + DISCOUNT_FACTOR * valueMatrix[rc][rr];

    return 0.7 * move + 0.15 * revMv + 0.15 * stay;
}

string movement(string attemptedMove) {
    array<string, 3> moveset;
    array<double, 3> moveProbabilities = { 0.7, 0.15, 0.15 }; // forward, reverse, stall

    if (attemptedMove == "UP") {
        moveset = { "UP", "DOWN", "STALL" };
    }
    else if (attemptedMove == "DOWN") {
        moveset = { "DOWN", "UP", "STALL" };
    }
    else if (attemptedMove == "LEFT") {
        moveset = { "LEFT", "RIGHT", "STALL" };
    }
    else if (attemptedMove == "RIGHT") {
        moveset = { "RIGHT", "LEFT", "STALL" };
    }
    else {
        return "INVALID";
    }

    double probability = (double)rand() / RAND_MAX; // random number 0→1
    double cumulative = 0.0;
    for (int j = 0; j < 3; j++) {
        if (probability > moveProbabilities[j]) {
            probability = probability - moveProbabilities[j];
        }
        else {
            return moveset[j];
        }
    }
    return "INVALID";
}

double applymovement(string appliedMove, vector<vector<Grid>>& GameBoard, int x, int y) {
    printf("Applying Move: %s at Position (%d, %d)\n", appliedMove.c_str(), x, y);
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) {
        cout << "Out of bounds position!" << endl;
        return -1e9;
    }

    int new_x1 = x, new_y1 = y;  // intended move (70%)
    int new_x2 = x, new_y2 = y;  // stay in place (15%)
    int new_x3 = x, new_y3 = y;  // reverse move (15%)

    if (appliedMove == "^") {
        new_y1 = min(ROWS - 1, y + 1);
        new_y2 = y;
        new_y3 = max(0, y - 1);
    }
    else if (appliedMove == "v") {
        new_y1 = max(0, y - 1);
        new_y2 = y;
        new_y3 = min(ROWS - 1, y + 1);
    }
    else if (appliedMove == "<") {
        new_x1 = max(0, x - 1);
        new_x2 = x;
        new_x3 = min(COLS - 1, x + 1);
    }
    else if (appliedMove == ">") {
        new_x1 = min(COLS - 1, x + 1);
        new_x2 = x;
        new_x3 = max(0, x - 1);
    }

    double reward1 = GameBoard[new_x1][new_y1].cost;
    double reward2 = GameBoard[new_x2][new_y2].cost;
    double reward3 = GameBoard[new_x3][new_y3].cost;

    return 0.7 * reward1 + 0.15 * reward2 + 0.15 * reward3;
}


void printGrid(vector<vector<Grid>>& GameBoard, int xpos, int ypos) {
    for (int row = ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            const string cellType = typeToString(GameBoard[col][row].type);
            if (col == xpos && row == ypos) {
                printf("[P %5s]  ", cellType.c_str());
            }
            else {
                printf("[%7s]  ", cellType.c_str());
            }
        }
        cout << endl;
    }
    cout << endl;
}

void printPolicy(vector<vector<string>> policyGrid, vector<vector<double>> valueGrid) {
    // Print policy arrows
    for (int row = ROWS - 1; row >= 0; row--) {   // y
        for (int col = 0; col < COLS; col++) {    // x
            if (row == 0 && col == 0) {
                cout << "[P " << policyGrid[col][row] << " ] ";

            }
            else {
                cout << "[  " << policyGrid[col][row] << " ] ";
            }
        }
        cout << endl;
    }
    // Print values
    for (int row = ROWS - 1; row >= 0; row--) {   // y
        for (int col = 0; col < COLS; col++) {    // x
            if (row == 0 && col == 0) {
                cout << "[P " << setw(8) << valueGrid[col][row] << "] ";  // Changed from valueGrid[row][col]
            }
            else {
                cout << "[ " << setw(9) << valueGrid[col][row] << "] ";   // Changed from valueGrid[row][col]
            }
        }
        cout << endl;
    }
}

void valuePolicy(vector<vector<Grid>>& GameBoard, vector<vector<double>>& valueMatrix, vector<vector<string>>& policy) {
    string moves[4] = { "<", ">", "^", "v" };
    const int dc[4] = { -1, 1, 0, 0 };  // Up, Down
    const int dr[4] = { 0, 0, -1, 1 };  // Left, Right


    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            double bestValue = -1e9;
            int bestAction = 2;//Up unless otherwise, greedy choice
            // Check each direction and pick the one with highest value
            for (int action = 0; action < 4; action++) {
                double actionValue = nextAction(r, c, action, valueMatrix, GameBoard);
                //cout << "Action Value at " << (c ) << "," << (r) << " for action " << moves[action] << " is " << actionValue << endl;
                // Pick action leading to highest value
                if (actionValue > bestValue) {
                    bestValue = actionValue;
                    bestAction = action;
                }
                else if (fabs(actionValue - bestValue) < 1e-9) {
                    if (action % 2 == 0)
                        bestAction = action;
                }
            }
            //Prints best action for each cell, it is an X if something seriously wrong has happened
            policy[c][r] = (bestAction >= 0) ? moves[bestAction] : "X";
        }
    }
}

void valueIteration(int horizon, vector<vector<Grid>> GameBoard, vector<vector<double>>& valueMatrix, vector<vector<string>>& policy) {
    vector<vector<double>> newMatrix(COLS, vector<double>(ROWS, 0.0));

    //Initialize valueMatrix
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            valueMatrix[c][r] = GameBoard[c][r].cost;
        }
    }
    for (int i = 1; i <= horizon; i++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                // if (GameBoard[r][c].type == MONEY || GameBoard[r][c].type == WUMPUS) {
                //     newMatrix[r][c] = GameBoard[r][c].cost;
                //     continue;
                // }
                double bestValue = -1e9;
                for (int action = 0; action < 4; action++) {
                    double expectedValue = nextAction(r, c, action, valueMatrix, GameBoard);
                    bestValue = max(bestValue, expectedValue);
                }
                newMatrix[c][r] = bestValue;
            }
        }
        // Update valueMatrix properly (since it's a reference)
        valueMatrix.assign(newMatrix.begin(), newMatrix.end());
    }
}

tuple<vector<vector<string>>, vector<vector<double>>> policyInitialization(vector<vector<Grid>>& GameBoard) {
    vector<vector<string>> policyGrid(COLS, vector<string>(ROWS));
    vector<vector<double>> valueGrid(COLS, vector<double>(ROWS));
    array<string, 4> policy = { "^", "v", "<", ">" };

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            policyGrid[col][row] = policy[rand() % 4];
            valueGrid[col][row] = GameBoard[col][row].cost;
        }
    }

    //printPolicy(policyGrid, valueGrid);
    return make_tuple(policyGrid, valueGrid);
}

double policyValue(string action, int col, int row, vector<vector<double>>& valueGrid) {
    int x1 = col, y1 = row;  // intended (70%)
    int x2 = col, y2 = row;  // stay (15%)
    int x3 = col, y3 = row;  // reverse (15%)

    if (action == "^") {
        y1 = min(ROWS - 1, row + 1);
        y2 = row;
        y3 = max(0, row - 1);
    }
    else if (action == "v") {
        y1 = max(0, row - 1);
        y2 = row;
        y3 = min(ROWS - 1, row + 1);
    }
    else if (action == "<") {
        x1 = max(0, col - 1);
        x2 = col;
        x3 = min(COLS - 1, col + 1);
    }
    else if (action == ">") {
        x1 = min(COLS - 1, col + 1);
        x2 = col;
        x3 = max(0, col - 1);
    }

    return 0.7 * valueGrid[x1][y1] + 0.15 * valueGrid[x2][y2] + 0.15 * valueGrid[x3][y3];
}

void policyEvaluation(vector<vector<Grid>>& GameBoard, vector<vector<string>>& policyGrid, vector<vector<double>>& valueGrid, int timeHorizon) {
    /*
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            valueGrid[col][row] = GameBoard[col][row].cost;
        }
    }
    */
    for (int t = 0; t < timeHorizon; t++) {
        vector<vector<double>> newValueGrid = valueGrid;
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                string action = policyGrid[col][row];
                double expectedValue = policyValue(action, col, row, valueGrid);
                newValueGrid[col][row] = GameBoard[col][row].cost + (DISCOUNT_FACTOR * expectedValue);
            }
        }

        valueGrid = newValueGrid;
        //printPolicy(policyGrid, valueGrid);
    }
}

void PolicyImprovement(vector<vector<Grid>>& GameBoard, vector<vector<string>>& policyGrid, vector<vector<double>>& valueGrid, int timeHorizon) {
    bool policyStable = false;
    array<string, 4> policy = { "^", "v", "<", ">" };

    while (!policyStable) {
        // PHASE 1: Policy Evaluation
        policyEvaluation(GameBoard, policyGrid, valueGrid, timeHorizon);
        //printPolicy(policyGrid, valueGrid);
        //cout << "Policy Improvement Iteration: " << ++PolicyCheck << endl;
        vector<vector<string>> newPolicyGrid = policyGrid;
        // PHASE 2: Policy Improvement
        policyStable = true;
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS; col++) {
                string oldAction = policyGrid[col][row];
                double maxValue = GameBoard[col][row].cost + (policyValue(oldAction, col, row, valueGrid) * DISCOUNT_FACTOR);
                string bestAction = oldAction;


                // Try all actions
                for (int p = 0; p < POLICY_SIZE; p++) {
                    string action = policy[p];
                    double expectedValue = policyValue(action, col, row, valueGrid);
                    double replacementValue = GameBoard[col][row].cost + (DISCOUNT_FACTOR * expectedValue);
                    //cout << "Action : " << action << " gives value: " << replacementValue << endl;
                    if (replacementValue > maxValue) {
                        maxValue = replacementValue;
                        bestAction = action;
                    }
                }

                // Update policy if better action found
                if (bestAction != oldAction) {
                    //cout << "Policy updated at (" << col << "," << row << "): ";
                    //cout << "Old action: " << oldAction << "Gave value of: " << valueGrid[col][row] << " New action: " << bestAction << " with value: " << maxValue << " at (" << col << "," << row << ")" << endl;
                    //printPolicy(policyGrid, valueGrid);
                    newPolicyGrid[col][row] = bestAction;


                    policyStable = false;
                }
            }
        }
        policyGrid = newPolicyGrid;

        //printPolicy(policyGrid, valueGrid);
    }
}

int main() {
    int xpos = 0, ypos = 0, timeHorizon1 = 5, timeHorizon2 = 100;

    vector<vector<Grid>> GameBoard;
    initializeGrid(GameBoard);
    srand(time(NULL));

    printGrid(GameBoard, xpos, ypos);
    cout << "Wumpus World Initialized!!" << endl;

    vector<vector<string>> policy(COLS, vector<string>(ROWS, ""));
    vector<vector<string>> policy2(COLS, vector<string>(ROWS, ""));
    vector<vector<double>> valueMatrix(COLS, vector<double>(ROWS, 0.0));
    vector<vector<double>> valueMatrix2(COLS, vector<double>(ROWS, 0.0));
    valueIteration(50, GameBoard, valueMatrix, policy);
    valuePolicy(GameBoard, valueMatrix, policy);
    cout << "Value Iteration: Policy and Value Matrix for Time Horizon 50 " << endl << endl;
    printPolicy(policy, valueMatrix);

    valueIteration(100, GameBoard, valueMatrix2, policy2);
    valuePolicy(GameBoard, valueMatrix2, policy2);
    cout << "Value Iteration: Policy and Value Matrix for Time Horizon 100 " << endl << endl;
    printPolicy(policy2, valueMatrix2);


    vector<vector<string>> policyGrid;
    vector<vector<double>> valueGrid;
    tie(policyGrid, valueGrid) = policyInitialization(GameBoard);

    // Run Policy Iteration
    PolicyImprovement(GameBoard, policyGrid, valueGrid, timeHorizon1);

    cout << "\nFinal Policy and Values for Time Horizon 50:" << endl;
    printPolicy(policyGrid, valueGrid);
    //printGrid(GameBoard, xpos, ypos);

    initializeGrid(GameBoard);
    Map1(GameBoard);
    tie(policyGrid, valueGrid) = policyInitialization(GameBoard);

    cout << "\nRe-initialized Policy and Values for Time Horizon 100:" << endl;

    PolicyImprovement(GameBoard, policyGrid, valueGrid, timeHorizon2);

    cout << "\nFinal Policy and Values for Time Horizon 100:" << endl;
    printPolicy(policyGrid, valueGrid);
    //printGrid(GameBoard, xpos, ypos);
    printf("Wumpus World Completed!!!\n");

    return 0;
}