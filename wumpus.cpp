#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <array>
#include <tuple>


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

tuple<int, int> applymovement(string appliedMove, vector<vector<Grid>>& GameBoard, int x, int y) {
	printf("Applying Move: %s at Position (%d, %d)\n", appliedMove.c_str(), x, y);
    if (x < 0 || x >= COLS || y < 0 || y >= ROWS) {
        cout << "Out of bounds position!" << endl;
        return make_tuple(0, 0);
    }

    if (appliedMove == "STALL") {
        return make_tuple(x, y);
    }
    else if (appliedMove == "DOWN") {
        y = max(0, y - 1);
    }
    else if (appliedMove == "UP") {
        y = min(ROWS - 1, y + 1);
    }
    else if (appliedMove == "LEFT") {
        x = max(0, x - 1);
    }
    else if (appliedMove == "RIGHT") {
        x = min(COLS - 1, x + 1);
    }
	printf("New Position after Move: (%d, %d)\n", x, y);
    return make_tuple(x, y);
}


void printGrid(vector<vector<Grid>>& GameBoard, int xpos, int ypos) {
    for (int i = COLS - 1; i >= 0; i--) {
        for (int j = 0; j < ROWS; j++) {
            if (j == xpos && i == ypos) {
                printf("[P%3s]  ", (typeToString(GameBoard[j][i].type).c_str()));
            }
            else {
                printf("[%4s]  ", (typeToString(GameBoard[j][i].type).c_str()));
            }
        }
        cout << endl;
    }
}

int main() {
    double valueMatrix[ROWS][COLS] = { 0.0 };
	int xpos, ypos;
    xpos = 0;
	ypos = 0;
    vector<vector<Grid>> GameBoard;
    initializeGrid(GameBoard);
    srand(time(NULL));
    /*
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << "Grid[" << i << "][" << j << "] - Type: " << typeToString(GameBoard[i][j].type)
                << ", Cost: " << GameBoard[i][j].cost << endl;
        }
    }
    */

    // printGrid(GameBoard, xpos, ypos);
    // cout << "Wumpus World Initialized!!" << endl;
    // string appliedMove = "";
    // for (int i = 0; i <= 5; i++) {
    //     appliedMove = movement("UP");
    //     tie(xpos, ypos) = applymovement(appliedMove, GameBoard, xpos, ypos);
    //     cout << "Applied Move is: " << appliedMove << " Xpos: " << xpos << " Ypos: " << ypos << endl;
    //     printGrid(GameBoard, xpos, ypos);
    //     cout << endl;
    // }

    cout << "Testing value iteration of horizon 50..." << endl;
    valueIteration(50, GameBoard, valueMatrix);
    for (int i = COLS - 1; i >= 0; i--) {
        for (int j = 0; j < ROWS; j++) {
            cout << fixed << setprecision(2) << setw(8) << valueMatrix[j][i] << " ";
        }
        cout << endl;
    }
    vector<vector<string>> policy(ROWS, vector<string>(COLS, ""));
    cout << "Determining optimal policy from value iteration..." << endl;
    valuePolicy(GameBoard, valueMatrix, &policy);
    for (int i = COLS - 1; i >= 0; i--) {
        for (int j = 0; j < ROWS; j++) {
            cout << setw(6) << policy[j][i] << " ";
        }
        cout << endl;
    }
    return 0;
}