#include <iostream>
#include <vector>
using namespace std;
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
    bool operator ==(const Grid& other) const {
        return (type == other.type);
    }
};

int main(){
    return 0;
}