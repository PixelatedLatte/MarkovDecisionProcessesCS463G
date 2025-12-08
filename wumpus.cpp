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
    GameBoard[5][2].type = STINK;

    GameBoard[4][2].type = WUMPUS;
    GameBoard[4][2].cost = -10000;
}

double nextAction(int r, int c, int action, double valueMatrix[ROWS][COLS], vector<vector<Grid>>& GameBoard) {
    // The possible movements in vector form
    const int dr[4] = { -1, 1, 0, 0 }; //Up, Down
    const int dc[4] = { 0, 0, -1, 1 }; //Left, Right

    //Reverse action of the one we are evluating, for the 15% chance to take the reverse action
    int rev = (action % 2 == 0) ? action + 1 : action - 1;

    int nr = r + dr[action];
    int nc = c + dc[action];

    int rr = r + dr[rev];
    int rc = c + dc[rev];

    //For if the move is valid or not, forward or reversed
    bool reverseInvalid = (rr < 0 || rr >= ROWS || rc < 0 || rc >= COLS);
    bool forwardInvalid = (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS);

    if(reverseInvalid){
        rr = r;
        rc = c;
    }

    if(forwardInvalid){
        nr = r;
        nc = c;
    }

    auto futureV = [&](int rr, int rc) -> double{
        if (GameBoard[rr][rc].type == WUMPUS){
            return -10000.0;
        }
        return valueMatrix[rr][rc];
    };

    auto reward = [&](int rr, int rc) -> double {
        if (GameBoard[rr][rc].type == WUMPUS) return -10000.0;
        return GameBoard[rr][rc].cost;   // cost of stepping onto that square
    };

    double stay = reward(r, c) + DISCOUNT_FACTOR * futureV(r,c);
    double move = reward(nr, nc) + DISCOUNT_FACTOR * futureV(nr, nc);
    double revMove = reward(rr, rc) + DISCOUNT_FACTOR * futureV(rr, rc);

    return 0.7*move + 0.15*revMove + 0.15*stay;
}

void valueIteration(int horizon, vector<vector<Grid>>& GameBoard, double valueMatrix[ROWS][COLS]){
    double newMatrix[ROWS][COLS] = { 0.0 };
    for (int i = 1; i <= horizon; i++){
        for (int r = 0; r < ROWS; r++){
            for (int c = 0; c < COLS; c++){
                double bestValue = -1e9;

                if (GameBoard[r][c].type == WUMPUS) {
                    newMatrix[r][c] = -10000.0;
                    continue;
                }

                for (int action = 0; action < 4; action++){
                    double expectedValue = nextAction(r, c, action, valueMatrix, GameBoard);
                    bestValue = max(bestValue, expectedValue);
                }
                newMatrix[r][c] = bestValue;
            }
        }
        memcpy(valueMatrix, newMatrix, sizeof(newMatrix));
    }
}

void valuePolicy(vector<vector<Grid>>& GameBoard, double valueMatrix[ROWS][COLS], vector<vector<string>>* policy) {
    string moves[4] = { "^", "v", "<", ">"};
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            double bestValue = -1e9;
            int bestAction = -1;
            
            for (int action = 0; action < 4; action++) {
                double expectedValue = nextAction(r, c, action, valueMatrix, GameBoard);
                if (expectedValue > bestValue) {
                    bestValue = expectedValue;
                    bestAction = action;
                }
            }
            (*policy)[r][c] = (bestAction >= 0) ? moves[bestAction] : "X";
        }
    }
}

string movement(string attemptedMove) {
    array<string, 3> moveset;
    array<double, 3> moveProbabilities = { 0.7, 0.15, 0.15 }; // forward, reverse, stall

    if (attemptedMove == "UP") {
        moveset = { "UP", "DOWN", "STALL" };
    } else if (attemptedMove == "DOWN") {
        moveset = { "DOWN", "UP", "STALL" };
    } else if (attemptedMove == "LEFT") {
        moveset = { "LEFT", "RIGHT", "STALL" };
    } else if (attemptedMove == "RIGHT") {
        moveset = { "RIGHT", "LEFT", "STALL" };
    } else {
        return "INVALID";
    }

    double probability = (double)rand() / RAND_MAX; // random number 0→1
    double cumulative = 0.0;
    for (int j = 0; j < 3; j++) {
        if (probability > moveProbabilities[j]) {
            probability = probability - moveProbabilities[j];
        } else {
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
                printf("[P %2s]  ", cellType.c_str());
            }
            else {
                printf("[%4s]  ", cellType.c_str());
            }
        }
        cout << endl;
    }
    cout << endl;
}

void printPolicy(vector<vector<string>> policyGrid, vector<vector<double>> valueGrid) {
    for (int row = ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            if (row == 0 && col == 0) {
                cout << "[P " << policyGrid[col][row] << " ] ";
            }
            else {
                cout << "[  " << policyGrid[col][row] << " ] ";
            }
        }
        cout << endl;
    }

    for (int row = ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            if (row == 0 && col == 0) {
                cout << "[P " << setw(6) << valueGrid[col][row] << "] ";
            }
            else {
                cout << "[ " << setw(7) << valueGrid[col][row] << "] ";
            }
        }
        cout << endl;
    }
}

tuple<vector<vector<string>>, vector<vector<double>>> policyInitialization(vector<vector<Grid>>& GameBoard) {
    vector<vector<string>> policyGrid(ROWS, vector<string>(COLS));
    vector<vector<double>> valueGrid(ROWS, vector<double>(COLS));
    array<string, 4> policy = { "^", "v", "<", ">" };

    for (int row = ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            policyGrid[col][row] = policy[rand() % 4];
        }
    }

    for (int row = ROWS - 1; row >= 0; row--) {
        for (int col = 0; col < COLS; col++) {
            valueGrid[col][row] = GameBoard[col][row].cost;
        }
    }

    printPolicy(policyGrid, valueGrid);
    return make_tuple(policyGrid, valueGrid);
}

void policyEvaluation(vector<vector<Grid>>& GameBoard, vector<vector<string>>& policyGrid, vector<vector<double>>& valueGrid, double gamma, int timeHorizon) {
    for (int step = 0; step < timeHorizon; step++) {
        vector<vector<double>> newValueGrid(ROWS, vector<double>(COLS));

        cout << "\n=== Iteration " << step + 1 << " ===" << endl;

        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                int xpos = i;
                int ypos = j;

                array<string, 4> moves = { "^", "v", "<", ">" };
                double maxExpectedValue = -1e9;
                string bestMove = "^";

                for (int k = 0; k < 4; k++) {
                    string attemptedMove = moves[k];

                    double immediateReward = applymovement(attemptedMove, GameBoard, xpos, ypos);

                    int new_x1 = xpos, new_y1 = ypos;
                    int new_x2 = xpos, new_y2 = ypos;
                    int new_x3 = xpos, new_y3 = ypos;

                    if (attemptedMove == "^") {
                        new_y1 = min(ROWS - 1, ypos + 1);
                        new_y2 = ypos;
                        new_y3 = max(0, ypos - 1);
                    }
                    else if (attemptedMove == "v") {
                        new_y1 = max(0, ypos - 1);
                        new_y2 = ypos;
                        new_y3 = min(ROWS - 1, ypos + 1);
                    }
                    else if (attemptedMove == "<") {
                        new_x1 = max(0, xpos - 1);
                        new_x2 = xpos;
                        new_x3 = min(COLS - 1, xpos + 1);
                    }
                    else if (attemptedMove == ">") {
                        new_x1 = min(COLS - 1, xpos + 1);
                        new_x2 = xpos;
                        new_x3 = max(0, xpos - 1);
                    }

                    double futureValue = 0.7 * valueGrid[new_x1][new_y1] +
                        0.15 * valueGrid[new_x2][new_y2] +
                        0.15 * valueGrid[new_x3][new_y3];

                    double expectedValue = immediateReward + gamma * futureValue;

                    if (expectedValue > maxExpectedValue) {
                        maxExpectedValue = expectedValue;
                        bestMove = attemptedMove;
                    }
                }

                policyGrid[xpos][ypos] = bestMove;
                newValueGrid[i][j] = maxExpectedValue;
            }
        }

        valueGrid = newValueGrid;

        if (step % 10 == 0 || step == timeHorizon - 1) {
            cout << "\nPolicy and Values after iteration " << step + 1 << ":" << endl;
            printPolicy(policyGrid, valueGrid);
        }
    }
}

int main() {
    double valueMatrix[ROWS][COLS] = { 0.0 };
    int xpos = 0, ypos = 0, timeHorizon = 100;
    double gamma = 0.95;

    vector<vector<Grid>> GameBoard;
    initializeGrid(GameBoard);
    Map1(GameBoard);
    srand(time(NULL));

    printGrid(GameBoard, xpos, ypos);
    cout << "Wumpus World Initialized!!" << endl;

    vector<vector<string>> policyGrid;
    vector<vector<double>> valueGrid;
    tie(policyGrid, valueGrid) = policyInitialization(GameBoard);

    policyEvaluation(GameBoard, policyGrid, valueGrid, gamma, timeHorizon);

    cout << "\n=== Final Policy and Values ===" << endl;
    printPolicy(policyGrid, valueGrid);
    printGrid(GameBoard, xpos, ypos);

    return 0;
}