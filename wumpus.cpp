#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <array>
#include <tuple>
#include<cmath>
#include <cstdlib>
#include <ctime>
// Initialize packages, namespaces, and constants values + declarations
using namespace std;
void initializeGrid(vector<vector<class Grid>>&);
void Map1(vector<vector<class Grid>>&);
void Map2(vector<vector<class Grid>>&);

const int ROWS = 6;
const int COLS = 6;
const double DISCOUNT_FACTOR = 0.95;
const int POLICY_SIZE = 4;
int PolicyCheck = 0;

// Define state types
enum squareType {
    EMPTY,
    MONEY,
    STINK,
    ENTRY,
    WUMPUS
};

// Define Grid class
class Grid {
public:
    squareType type;
    int cost;
    string name;
};

// Convert squareType to string for display, focus on non-empty spaces
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

// Initialize the game board with empty grids and set up the map
void initializeGrid(vector<vector<Grid>>& GameBoard) {
    GameBoard.resize(ROWS, vector<Grid>(COLS));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            GameBoard[j][i].type = EMPTY;
            GameBoard[j][i].cost = 0;
        }
    }
}

// Define Map 1 configuration, locations of money, wumpus, stinks, and entry point
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

// Define Map 2 configuration, locations of money, wumpus, stinks, and entry point
void Map2(vector<vector<Grid>>& GameBoard) {
    GameBoard[4][4].type = MONEY;
    GameBoard[4][4].cost = 1700;

    GameBoard[0][2].type = MONEY;
    GameBoard[0][2].cost = 20;

    GameBoard[0][3].type = MONEY;
    GameBoard[0][3].cost = -5;

    GameBoard[1][5].type = MONEY;
    GameBoard[1][5].cost = 10;

    GameBoard[5][1].type = MONEY;
    GameBoard[5][1].cost = -20;

    GameBoard[0][0].type = ENTRY;

    GameBoard[4][1].type = STINK;
    GameBoard[4][3].type = STINK;
    GameBoard[3][2].type = STINK;
    GameBoard[5][2].type = STINK;

    GameBoard[4][2].type = WUMPUS;
    GameBoard[4][2].cost = -10000;
}
// Function to calculate expected value of taking an action from (r, c)
double nextAction(int r, int c, int action, vector<vector<double>>& valueMatrix, vector<vector<Grid>>& GameBoard){
    // Directions match your indexing
    const int dc[4] = { -1,  1,  0,  0 }; // left, right
    const int dr[4] = { 0,  0,  1, -1 }; // up, down  (row increases upward)
    // Reverse action
    int rev = (action % 2 == 0) ? action + 1 : action - 1;
    //Next action given you go forward
    int nc = c + dc[action];
    int nr = r + dr[action];
    //Next action given you go in the reverse direction
    int rc = c + dc[rev];
    int rr = r + dr[rev];
    // Clamp to boundaries
    if (nc < 0 || nc >= COLS || nr < 0 || nr >= ROWS) { nc = c; nr = r; }
    if (rc < 0 || rc >= COLS || rr < 0 || rr >= ROWS) { rc = c; rr = r; }
    //Values from the valueMatrix for stay, move, and reverse moves
	double stay = valueMatrix[c][r];
    double move = valueMatrix[nc][nr];
    double revMv = valueMatrix[rc][rr];
    return 0.7 * move + 0.15 * revMv + 0.15 * stay;
}
// Creates a policy from a valueMatrix by choosing the best action at each state
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
                // Pick action leading to highest value
                if (actionValue > bestValue) {
                    bestValue = actionValue;
                    bestAction = action;
                }
                //Tie breaker, prefers up or left(left almost never happens in tiebreaker)
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
// Algorithm to perform value iteration over a given time horizon
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
                double bestValue = -1e9;
                for (int action = 0; action < 4; action++) {//For each action
                    double expectedValue = nextAction(r, c, action, valueMatrix, GameBoard);//Expected value coming from nextAction
					expectedValue = GameBoard[c][r].cost + (DISCOUNT_FACTOR * expectedValue);//Updated Expected action from rest of formula
                    bestValue = max(bestValue, expectedValue);
                }
                newMatrix[c][r] = bestValue;
            }
        }
        // Update valueMatrix properly (since it's a reference)
        valueMatrix.assign(newMatrix.begin(), newMatrix.end());
    }
}
void printGrid(vector<vector<Grid>> GameBoard, int xpos, int ypos) {
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
	// Print policy arrows, with player position marked for reference
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
	// Print value grid, with player position marked for reference
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

// For POLICY ITERATION ONLY, initialize random policy grid and value grid matching current map costs
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
    return make_tuple(policyGrid, valueGrid);
}

// Calculate expected value of taking an action from a given state
double policyValue(string action, int col, int row, vector<vector<double>>& valueGrid) {
    int x1 = col, y1 = row; 
    int x2 = col, y2 = row;  
	int x3 = col, y3 = row;  // Initialize to current position 

	// If action pushes against boundary, stay in place, otherwise move accordingly
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

	// Return expected value based on transition probabilities (70% intended, 15% stall, 15% opposite direction)
    return 0.7 * valueGrid[x1][y1] + 0.15 * valueGrid[x2][y2] + 0.15 * valueGrid[x3][y3];
}

// Evaluate the current policy over a specified time horizon
void policyEvaluation(vector<vector<Grid>>& GameBoard, vector<vector<string>>& policyGrid, vector<vector<double>>& valueGrid, int timeHorizon) {
	// Does NOT modify current policy, only updates value grid based on it
	// Reset value grid to initialized costs
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            valueGrid[col][row] = GameBoard[col][row].cost;
        }
    }
	// Iterate to update value grid based on current policy
	for (int t = 0; t < timeHorizon; t++) { // For each time step (movement)
        vector<vector<double>> newValueGrid = valueGrid;
        for (int row = 0; row < ROWS; row++) { // For each state
            for (int col = 0; col < COLS; col++) {
				string action = policyGrid[col][row]; // Get action from current policy in said state
				double expectedValue = policyValue(action, col, row, valueGrid); // Calculate expected value of taking that action
                // Update value grid with new value (state value + discounted * expected value)
				newValueGrid[col][row] = GameBoard[col][row].cost + (DISCOUNT_FACTOR * expectedValue); 
            }
        }
        valueGrid = newValueGrid;
    }
}

// Improve the policy based on the evaluated value grid
void PolicyImprovement(vector<vector<Grid>>& GameBoard, vector<vector<string>>& policyGrid, vector<vector<double>>& valueGrid, int timeHorizon) {
	// Initialize policy to not stable, as well as posible actions for each state
    bool policyStable = false;
    array<string, 4> policy = { "^", "v", "<", ">" };
    // While theres still policy improvement to be made
    while (!policyStable) {
		// Evaluate current policy
        policyEvaluation(GameBoard, policyGrid, valueGrid, timeHorizon);
		// Create temp policy grid to store improvements + assume stable unless change found
        vector<vector<string>> newPolicyGrid = policyGrid;
        policyStable = true;
		for (int row = 0; row < ROWS; row++) { // Iterate through each state
            for (int col = 0; col < COLS; col++) {
				string oldAction = policyGrid[col][row]; // Get current action from policy and compute its value
                double maxValue = GameBoard[col][row].cost + (policyValue(oldAction, col, row, valueGrid) * DISCOUNT_FACTOR);
				string bestAction = oldAction; // Initialize best action as current action
                // Try all actions
				for (int p = 0; p < POLICY_SIZE; p++) { // compute value for each possible action
                    string action = policy[p];
                    double expectedValue = policyValue(action, col, row, valueGrid);
                    double replacementValue = GameBoard[col][row].cost + (DISCOUNT_FACTOR * expectedValue);
					if (replacementValue > maxValue) { // If better action found, update best action and max value
                        maxValue = replacementValue;
                        bestAction = action;
                    }
                }
                // Update temp grid policy if better action found
                if (bestAction != oldAction) {
                    newPolicyGrid[col][row] = bestAction;
                    policyStable = false;
                }
            }
        }
        policyGrid = newPolicyGrid;
    }
}

int main() {
	srand(time(NULL)); // Seed random number generator

	vector<int> horizons = { 50, 100 }; // Time horizons to test
    vector<pair<string, void(*)(vector<vector<Grid>>&)>> maps = {
        {"Map1", Map1}
		//,{"Map2", Map2} // Uncomment to include Map2
    };

    vector<vector<Grid>> GameBoard;

    // Initialize and display the world
    initializeGrid(GameBoard);
    Map1(GameBoard);
    printGrid(GameBoard, 0, 0);
    cout << "Wumpus World Initialized!!\n" << endl;

	// Run Policy Iteration and Value Iteration for all combinations of maps (currently only 1 map) and horizons
    for (const auto& [mapName, mapFunc] : maps) {
        for (int horizon : horizons) {
            cout << endl << "Policy Iteration Results for " << mapName << " with time horizon " << horizon << endl;

            initializeGrid(GameBoard);
            mapFunc(GameBoard);

            vector<vector<string>> policyGrid;
            vector<vector<double>> valueGrid;
            tie(policyGrid, valueGrid) = policyInitialization(GameBoard);

            PolicyImprovement(GameBoard, policyGrid, valueGrid, horizon);
            printPolicy(policyGrid, valueGrid);

			cout << endl << "Value Iteration Results for " << mapName << " with time horizon " << horizon << endl;

            initializeGrid(GameBoard);
            mapFunc(GameBoard);

            tie(policyGrid, valueGrid) = policyInitialization(GameBoard);

			valueIteration(horizon, GameBoard, valueGrid, policyGrid);
			valuePolicy(GameBoard, valueGrid, policyGrid);
			printPolicy(policyGrid, valueGrid);
        }
        cout << endl;
    }

    cout << "Wumpus World Completed!!!" << endl;
    return 0;
}