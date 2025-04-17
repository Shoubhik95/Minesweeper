#include <iostream>  //input/output operations
#include <vector>    //use of vectors(dynamic array) that will store the board cells
#include <ctime>     
#include <cstdlib>   //provides functions for generating random numbers( functions - rand and srand)
#include <limits>    //for setting the limits of the random number generator
#include <iomanip>   //for aligning the board cells 

using namespace std; 

//dimensions of the board
const int WIDTH = 10;  // Width of the grid
const int HEIGHT = 10; // Height of the grid
const int NUM_MINES = 20; // Number of mines

//color codes for terminal
const string RESET = "\033[0m"; //reset terminal text formatting
const string RED = "\033[31m";   //   \033 in octal form,  31m where 31 is code for red and m means modifier
const string GREEN = "\033[32m"; 
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BOLD_GREEN = "\033[1;32m";
const string BOLD_RED = "\033[1;31m";  // Bold red for Game Over message


//These are ANSI escape codes that allow color formatting in the terminal.


//cell state
enum CellState {     //enum(enumeration) is a way to define a set of named constants  ,,, cellstate is an enum that defines the possible states of cell on board
    HIDDEN,          //cell not yet revealed
    REVEALED,        //cell revealed
    FLAGGED          //cell flagged as mine
};


//cell structure

struct Cell{
    bool isMine;       //is this cell a mine?
    int adjacentMines; //number of adjacent mines
    CellState state;   // state of the cell
};

//board structure

void initializeBoard(vector<vector<Cell>>&board){
    //set all cells to hidden and not mine
    for(int i=0; i<HEIGHT; i++){
        for(int j=0; j<WIDTH; j++){
            board[i][j].isMine= false;     //non mines
            board[i][j].adjacentMines = 0;
            board[i][j].state = HIDDEN;    //hidden
        }
    }

    //now randomly place mines
    srand(time(0));            //seed random number generator with current time
    int minesPlaced= 0;
    while (minesPlaced < NUM_MINES){  //to place 20 mines
        int x = rand() % WIDTH;       //generate random x coordinate between 1 and WIDTH  ex:any random like 534262 % 10 i.e width gives remainder as 2
        int y = rand() % HEIGHT;

        if (!board[y][x].isMine){     //this check if randomly chosen cell is not a mine
            board[y][x].isMine = true; //this places the mine by setting isMine to true
            minesPlaced++;             //increment minesPlaced by 1

            for(int i = -1; i <= 1; i++){     //loop through adjacent row cells
                for(int j = -1; j <= 1; j++){  // loop through adjacent column cells
                    int nx = x + j;        // get neighbour's column index
                    int ny = y + i;        // get neighbour's row index
                    if (nx >= 0 && ny>=0 && nx <WIDTH && ny < HEIGHT){  // check if neighbour is within the board
                        board[ny][nx].adjacentMines++; // increment adjacentMines of neighbouring cell by 1
                    }
                }
            }    
        }
    }
}


//to print the board with current state of cells

void printBoard(const vector<vector<Cell>>& board){
    cout << "    "; //put some space so that column index align with cells
    for (int i=0; i < WIDTH ; i++){
        cout << setw(4) << i;      //set width x-axis
    }
    cout << endl;

    for (int i=0; i <HEIGHT; i++){
        cout <<setw(3)<< i << "   ";  //print row index  //ensure y-axis is aligned with rows
        for (int j=0; j <WIDTH; j++){    //each column
            const Cell& cell = board[i][j]; //cell is a reference to the Cell at position i,j in the board
            if (cell.state == HIDDEN){
                cout << "[ ] ";  // empty cell
            }else if(cell.state == FLAGGED){   //flagged cell
                cout << "["<<CYAN << "F" << RESET << "] ";  // flagged cell, print F in cyan colour
            }else if(cell.isMine){
                cout << "[" << RED << "*" << RESET << "] ";  // mine cell
            }else{
                string color = RESET;
                if (cell.adjacentMines == 1){
                    color = GREEN;
                }else if (cell.adjacentMines == 2){   //colours of blocks
                    color = BLUE;
                }else if (cell.adjacentMines == 3){
                    color = YELLOW;
                }else if (cell.adjacentMines >= 4){
                    color = MAGENTA;
                }
                cout << "[" << color << cell.adjacentMines << RESET << "] ";
            }
        }
        cout<< endl;
    }
}

//function to reveal cell

void revealCell(vector<vector<Cell>>& board, int x, int y) {
    if (board[y][x].state != HIDDEN) return;  //if not hidden , do nothing
    board[y][x].state = REVEALED;      //state of a cell of the board

    // If the selected cell has no adjacent mines, then reveal surrounding cells
    if (board[y][x].adjacentMines == 0 && !board[y][x].isMine) {  // checks if adjacent mines and the current mine is 0
        vector<pair<int, int>> toReveal = {{x, y}};

        //The loop continues as long as there are cells in toReveal. The last element is retrieved using back() and then removed from the list using pop_back().
        //toreveal is the vector defined above
        while (!toReveal.empty()) { 
            auto current = toReveal.back(); // Getting the pair from the back
            toReveal.pop_back();
            
            int cx = current.first;  // The x coordinate
            int cy = current.second; // The y coordinate

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int nx = cx + j;
                    int ny = cy + i;
                    if (nx >= 0 && ny >= 0 && nx < WIDTH && ny < HEIGHT && board[ny][nx].state == HIDDEN) {
                        board[ny][nx].state = REVEALED;   // if they are valid (in boundary and hidden), this reveals them.
                        if (board[ny][nx].adjacentMines == 0 && !board[ny][nx].isMine) {
                            toReveal.push_back({nx, ny});  // If a revealed neighboring cell has no adjacent mines and isn't a mine, it adds that cell to the toReveal list.
                        }
                    }
                }
            }
        }
    }
}


// Function to unreveal a cell
void unrevealCell(vector<vector<Cell>>& board, int x, int y) {
    if (board[y][x].state == REVEALED) {
        board[y][x].state = HIDDEN;  // Change the cell back to HIDDEN
    }
}

// Function to handle flagging and unflagging a cell
void flagCell(vector<vector<Cell>>& board, int x, int y) {
    if (board[y][x].state == HIDDEN) {
        board[y][x].state = FLAGGED;  // Flag the cell
    } else if (board[y][x].state == FLAGGED) {
        board[y][x].state = HIDDEN;   // Unflag the cell
    }
}

// Function to validate the user input
bool validateInput(char& action, int& x, int& y) {
    cout << "Enter your action (f for flag/unflag, r for reveal, u for unreveal), followed by coordinates (x y): example : r x_cordinate y_cordinate:";
    cin >> action >> x >> y;

    // Validate action
    if (action != 'r' && action != 'f' && action != 'u') {
        cout << "Invalid action! Please enter 'f' for flag/unflag, 'r' for reveal, or 'u' for unreveal." << endl;
        return false;
    }

    // Validate coordinates
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        cout << "Invalid coordinates! Please enter values between 0 and " << WIDTH<< " for x, and between 0 and " << HEIGHT << " for y." << endl;
        return false;
    }

    return true;
}    

// Function to print "Game Over" with red
void printGameOver() {
    cout << BOLD_RED << "*****************************" << endl;
    cout << "**** GAME OVER! YOU LOST ****" << endl;
    cout << "*****************************" << RESET << endl;
}


void printCongratulations() {
    cout << BOLD_GREEN << "******************************" << endl;
    cout << "**** CONGRATULATIONS! YOU WON ****" << endl;
    cout << "******************************" << RESET << endl;
}



// Main game loop
int main() {
    vector<vector<Cell>> board(HEIGHT, vector<Cell>(WIDTH));

    // Initialize the game board
    initializeBoard(board);

    int revealedCells = 0;
    bool gameOver = false;
    bool won = false;

    while (!gameOver) {
        // Print the current state of the board
        printBoard(board);

        // Get valid user input
        char action;
        int x, y;
        while (!validateInput(action, x, y)) {
            // Prompt the user until the input is valid
        }

        if (action == 'r') {
            // Reveal the selected cell
            if (board[y][x].isMine) {
                gameOver = true;
                cout << "You hit a mine! GAME OVER!" << endl; // Notify the player they hit a mine
                printGameOver(); // Print the Game Over message in red
            } else {
                revealCell(board, x, y);
                revealedCells++;
            }
        } else if (action == 'f') {
            // Flag or unflag the selected cell
            flagCell(board, x, y);
        } else if (action == 'u') {
            // Unreveal the selected cell
            unrevealCell(board, x, y);
        }

        // Check for win condition (if all non-mine cells are revealed)
        if (revealedCells == (WIDTH * HEIGHT - NUM_MINES)) {
            gameOver = true;
            won = true;
            cout << "Congratulations! You won!" << endl;
            printCongratulations();
        }

        // Pause the game for the user to see the board state
        cin.ignore();  // to clear out the leftover newline
        cin.get(); // wait for user to press Enter before continuing
    }

    return 0;
}