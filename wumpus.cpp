#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
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

double nextAction(int r, int c, int action, double valueMatrix[ROWS][COLS]) {
    // The possible movements in vector form
    const int dr[4] = { -1, 1, 0, 0 }; //Up, Down
    const int dc[4] = { 0, 0, -1, 1 }; //Left, Right

    //Reverse action of the one we are evluating, for the 15% chance to take the reverse action
    int rev = (action % 2 == 0) ? action + 1 : action - 1;

    int nr = r + dr[action];
    int nc = c + dc[action];

    int rr = r + dr[rev];
    int rc = c + dc[rev];

    //Lambda function to check for the position being in bounds or not
    auto clamp = [&](int &x, int &y){
        if (x < 0 || x >= ROWS || y < 0 || y >= COLS) {
            x = r;
            y = c;
        }
    };

    clamp(nr, nc);
    clamp(rr, rc);

    double stay = valueMatrix[r][c];
    double move = valueMatrix[nr][nc];
    double revMove = valueMatrix[rr][rc];

    return 0.70 * move + 0.15 * revMove + 0.15 * stay;
}

void valueIteration(int horizon, vector<vector<Grid>>& GameBoard, double valueMatrix[ROWS][COLS]){
    double newMatrix[ROWS][COLS] = { 0.0 };
    for (int i = 1; i <= horizon; i++){
        for (int r = 0; r < ROWS; r++){
            for (int c = 0; c < COLS; c++){
                double bestValue = -1e9;
                for (int action = 0; action < 4; action++){
                    double expectedValue = nextAction(r, c, action, valueMatrix);
                    if (expectedValue > bestValue){
                        bestValue = expectedValue;
                    }
                }
                newMatrix[r][c] = GameBoard[r][c].cost + DISCOUNT_FACTOR * bestValue;
            }
        }
        memcpy(valueMatrix, newMatrix, sizeof(newMatrix));
    }
}

void valuePolicy(vector<vector<Grid>>& GameBoard, double valueMatrix[ROWS][COLS], string policy[ROWS][COLS]) {
    string action[4] = { "^", "v", "<", ">"};
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            
            if(GameBoard[r][c].type == WUMPUS){
                cout << "Grid[" << r << "][" << c << "] is WUMPUS. No action." << endl;
                policy[r][c] = "WUMPUS";
                continue;
            }

            double bestValue = -1e9;
            int bestAction = -1;
            
            
            for (int action = 0; action < 4; action++) {
                double expectedValue = nextAction(r, c, action, valueMatrix);
                if (expectedValue > bestValue) {
                    bestValue = expectedValue;
                    bestAction = action;
                }
            }
            cout << "Best action for Grid[" << r << "][" << c << "] is: " << action[bestAction] << endl;
        }
    }
}

int main() {
    double valueMatrix[ROWS][COLS] = { 0.0 };
    vector<vector<Grid>> GameBoard;
    initializeGrid(GameBoard);
    /*
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            cout << "Grid[" << i << "][" << j << "] - Type: " << typeToString(GameBoard[i][j].type)
                << ", Cost: " << GameBoard[i][j].cost << endl;
        }
    }
    */

    for (int i = COLS-1; i >= 0; i--) {
        for (int j = 0; j < ROWS; j++) {
			printf("[%3s]  ", (typeToString(GameBoard[j][i].type).c_str()));
        }
		cout << endl;
	}
    cout << "Wumpus World Initialized!!" << endl;

    cout << "Testing value iteration of horizon 100..." << endl;
    valueIteration(100, GameBoard, valueMatrix);
    for (int i = COLS - 1; i >= 0; i--) {
        for (int j = 0; j < ROWS; j++) {
            cout << fixed << setprecision(2) << setw(8) << valueMatrix[j][i] << " ";
        }
        cout << endl;
    }

    string policy[ROWS][COLS] = { "" }; 
    cout << "Determining optimal policy from value iteration..." << endl;
    valuePolicy(GameBoard, valueMatrix, policy);
    return 0;
}