#include <iostream>
#include <vector>
using namespace std;

void initializeGrid(vector<vector<Grid>>&);

const int ROWS = 5;
const int COLS = 5;

//Defines our types of grid squares, default values are 0, can change the default values of squareType if need be
enum squareType{
    EMPTY,
    MONEY,
    STINK,
    ENTRY,
    WUMPUS
};

//Defines the structure of our grid, having whatever enumerated squareType and a value assigned to that square
class Grid{
    public:
        squareType type;
        int cost;
        string name;
    bool operator ==(const Grid& other) const {
        return (type == other.type);
    }
};

void initializeGrid(vector<vector<Grid>>& GameBoard){
    GameBoard.resize(ROWS, vector<Grid>(COLS));
    for(int i = 0; i < ROWS; i++){
        for(int j = 0; j < COLS; j++){
            GameBoard[i][j].type = EMPTY;
            GameBoard[i][j].cost = 0;
        }
    }
	Map1(GameBoard);
    //Uncomment to use Map2 instead of Map1
	//Map2(GameBoard); 
}

void Map1((vector<vector<Grid>>& GameBoard){
    // Places money in specific locations with associated costs (variant 1)
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

void Map2((vector<vector<Grid>>& GameBoard) {
	// Places money in specific locations with associated costs (variant 2)
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

int main(){
	vector<vector<Grid>> GameBoard;
	initializeGrid(GameBoard);

    return 0;
}