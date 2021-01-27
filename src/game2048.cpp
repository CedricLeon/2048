#include "../include/game2048.h"

// ********************************************************************* //
// ************************** GEGELATI FUNCTIONS *********************** //
// ********************************************************************* //

void game2048::setBoard(int i, int j, int newValue)
{
    try
    {
        this->board.setDataAt(typeid(int), 4*i + j, newValue);
    } catch (std::out_of_range& e)
    {
        std::cout << e.what() << std::endl;
    }
}

int game2048::getBoard(int i, int j)
{
    try {
        return (int)*(this->board.getDataAt(typeid(int), 4*i+j)).getSharedPointer<const int>();
    } catch (std::out_of_range& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
}

void game2048::doAction(uint64_t actionID)
{
    move(this->availableActions[actionID]);
}

std::vector<std::reference_wrapper<const Data::DataHandler>> game2048::getDataSources()
{
    // Return the game board
    auto result = std::vector<std::reference_wrapper<const Data::DataHandler>>();
    result.push_back(this->board);
    return result;
}

void game2048::reset(size_t seed, Learn::LearningMode mode)
{
    // Create seed from seed and mode
    size_t hash_seed = Data::Hash<size_t>()(seed) ^ Data::Hash<Learn::LearningMode>()(mode);
    // Reset the RNG
    this->rng.setSeed(hash_seed);

    // Reset the game
    this->initGame();
}

Learn::LearningEnvironment* game2048::clone() const
{
    return new game2048(*this);
}
bool game2048::isCopyable() const
{
    return true;
}
double game2048::getScore() const
{
    return score;
}

bool game2048::isTerminal() const
{
    return win || lost;
}

// ********************************************************************* //
// **************************** 2048 FUNCTIONS ************************* //
// ********************************************************************* //

/**
 * countDigits : function counting digits of a number
 * @param x : the number
 * @return number of digits
 */
int countDigits (int x)
{
    int count = 0;
    while (x != 0)
    {
        count++;
        x /= 10;
    }
    return count;
}
/**
 * printTile : function printing a number in 4 spaces depending on his number of digits
 * @param value : the number to print
 */
void game2048::printTile(int value)
{
    int size = countDigits(value);
    switch (size)
    {
        case 0:
            std::cout << "    ";               // 4 spaces
            break;
        case 1:
            std::cout << " " << value << "  "; // 3 spaces
            break;
        case 2:
            std::cout << " " << value << " ";  // 2 spaces
            break;
        case 3:
            std::cout << value << " ";         // 1 spaces
            break;
        case 4:
            std::cout << value;                // 0 spaces
            break;
        default:
            std::cout << "????";               // impossible case (if the AI has to play as far as possible)
            break;                             // remember to add 5, 6 and 7? spaces
    }
}

/**
 * printUI : Function printing the board and the score
 */
void game2048::printUI()
{
    // Browsing rows
    for(int row = 0; row < 4; ++row)
    {
        std::cout << "-----------------------------" << std::endl; // 29 - (Update if the goal is more than 2048)
        std::cout << "| ";
        // Browsing columns
        for(int col = 0; col < 4; ++col)
        {
            //int value = *(board2.getDataAt(typeid(int), 4*row+col));
            int value = getBoard(row, col);
            printTile(value);
            std::cout << " | ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------" << std::endl; // 29 -
    // Printing score
    std::cout << "  Score : " << score << std::endl;
    // Remind commands
    std::cout << "Z:Up, Q: Left, S:Down, D:Right, L:Leave" << std::endl;
}

/**
 * generateValueNewTile : Generating a new value between 2 and 4
 * @return the value, 2 with a probability of 3/4 and 4 in 1/4 cases
 */
int game2048::generateValueNewTile()
{
    int proba = this->rng.getInt32(0, 3);       // rand() : Good Idea ?
    if(proba == 0)
        return 4;
    return 2;
}

/**
 * generateUnoccupiedTile : Function looking for unoccupied tiles and returning one randomly selected
 * @return a std::pair<int, int> which correspond to row and column indexes
 *         if there is no unoccupied tile : losing condition, return <-1;-1>
 */
std::pair<int, int> game2048::generateUnoccupiedTile()
{
    // Looking for unoccupied tiles
    int nbUnoccupied = 0;
    std::pair<int, int> unoccupied[16];
    for(int row = 0; row < 4; ++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            if(this->getBoard(row, col) == 0)
            {
                unoccupied[nbUnoccupied].first = row;
                unoccupied[nbUnoccupied].second = col;
                nbUnoccupied ++;
            }
        }
    }
    // If there is no unoccupied tile : losing condition, return <-1;-1>
    if(nbUnoccupied == 0)
        return std::make_pair(-1, -1);

    // Return a random tile among unoccupied Tiles
    int randomTile = this->rng.getInt32(0, nbUnoccupied);
    return std::make_pair(unoccupied[randomTile].first, unoccupied[randomTile].second);
}

/**
 * generateNewTile : function adding a new tile (value : 2 or 4) in the board
 * @return true if the tile has been succesfully added
 *         false otherwise (lost condition)
 */
bool game2048::generateNewTile()
{
    // Finding which Tile is free
    std::pair<int, int> randomTile = generateUnoccupiedTile();
    if(randomTile.first == -1 || randomTile.second == -1)
        return false;
    // Computing its value
    this->setBoard(randomTile.first, randomTile.second, generateValueNewTile()); // board[randomTile.first][randomTile.second] = generateValueNewTile();
    return true;
}

/**
 * initGame : Initialise the board with 2 random tiles (value : 2 or 4)
 */
void game2048::initGame()
{
    for(int row = 0; row < 4; ++row)
        for(int col = 0; col < 4; ++col)
            this->setBoard(row, col, 0); // board[row][col] = 0;

    // 2 random Tiles start with a 2 or a 4
    for(int i = 0; i < 2; ++i)
        generateNewTile();

    // To init the game in winning condition ;)
    /*board[1][1] = 2;
    board[1][2] = 2;
    board[1][3] = 4;
    board[2][3] = 8;
    board[2][2] = 16;
    board[2][1] = 32;
    board[2][0] = 64;
    board[3][0] = 128;
    board[3][1] = 256;
    board[3][2] = 512;
    board[3][3] = 1024;*/
}

/**
 * resetTilesMoved : Reset the mergedTiles board (at each turn's end)
 */
void game2048::resetTilesMoved()
{
    for(int row = 0; row < 4; ++row)        // Range-based for loop warning, solution : for(auto & mergedTile : mergedTiles) ?
        for(int col = 0; col < 4; ++col)
            mergedTiles[row][col] = false;
}

/**
 * moveIsPossible : Check if a move between 2 tiles is possible, it checks :
 *                      - if the next tile isn't out of board
 *                      - if the next tile is 0
 *                      - if the current and the next tile have the same value
 * @param row : Current Tile row index
 * @param col : Current Tile column index
 * @param nextRow : Next Tile row index
 * @param nextCol : Next Tile column index
 * @return true if the move is possible, false otherwise
 */
bool game2048::moveIsPossible(int row, int col, int nextRow, int nextCol)
{
    // Check if next move is out of board
    if(nextRow < 0 || nextRow >= 4 || nextCol < 0 || nextCol >= 4)
    {
        // std::cout << "Checking leading out of board move" << std::endl;
        return false;
    }
    int valueNextTile = this->getBoard(nextRow, nextCol); // int valueNextTile = board[nextRow][nextCol];
    //std::cout << "row : " << row << "; col : " << col << "; nextRow : " << nextRow << "; nextCol : " << nextCol;
    //std::cout << "      current value : " << board[row][col] << "; valueNextTile : " << valueNextTile << std::endl;
    // Check if next Tile has same value than the current or if it's different from 0
    return valueNextTile == 0 || this->getBoard(row, col) == valueNextTile; // board[row][col] == valueNextTile;
}

/**
 * applyMove : Main function of the soft, browse many times the board to update the tiles (merge cases, move cases, etc...)
 *             Also add a new tile at turn's end if a move has be done
 *             Check win condition
 * @param direction : The direction (Up = 0, Right = 1, Down = 2, Left = 3) of the move
 * @return true if a move was made
 *         false if the move can't be done (ex : trying to go down as all tiles are on the bottom side of the board)
 *         false if ... the winning condition (2048 Tile) is met
 */
bool game2048::applyMove(int direction)
{
    // Initialise default board "travel" direction : from Up to Down and Left to Right
    int startLine = 0, startColumn = 0, lineStep = 1, columnStep = 1;
    if(direction == 2) // If Up, then start from bottom
    {
        startLine = 3;
        lineStep = -1;
    }else if (direction == 1) // If Left, start from right
    {
        startColumn = 3;
        columnStep = -1;
    }
    // When it goes Up, we decrease lines, but don't touch column
    // When it goes Right, don't touch lines but increase columns, etc ...
    int dirLine[] = {-1, 0, 1, 0};
    int dirColumn[] = {0, 1, 0, -1};

    bool movePossible, canAddTile = false;
    do
    {
        movePossible = false;
        // Browsing rows
        for(int row = startLine; row >=0 && row < 4; row += lineStep)
        {
            // Browsing columns
            for(int col = startColumn; col >= 0 && col < 4; col += columnStep)
            {
                // Calculate the next tile
                int nextRow = row + dirLine[direction];
                int nextCol = col + dirColumn[direction];
                // Check if the current Tile isn't 0 and if it can move
                if(this->getBoard(row, col) != 0 && moveIsPossible(row, col, nextRow, nextCol))
                {
                    int nextValue = this->getBoard(nextRow, nextCol); // board[nextRow][nextCol];
                    // Check if next Tile is 0 (in this case it's just a move)
                    if(nextValue == 0) // Case with just a move, not a merge
                    {
                        // Make the move ;)
                        this->setBoard(nextRow, nextCol, this->getBoard(row, col)); // board[nextRow][nextCol] = board[row][col];
                        this->setBoard(row, col, 0); // board[row][col] = 0;

                        // If the moved Tile was already merged, update the mergedTiles board
                        if(mergedTiles[row][col])
                        {
                            mergedTiles[nextRow][nextCol] = true;
                            mergedTiles[row][col] = false;
                            //std::cout << "MergedTiles updated to :  (" << nextRow << ";" << nextCol << ")" << std::endl;
                        }

                        // Indicate that a move had be done
                        movePossible = true;
                        canAddTile = true;
                    }
                        // If it's not just a move, check if each tiles had not already be merged
                    else if(nextValue == this->getBoard(row, col) && !mergedTiles[row][col] && !mergedTiles[nextRow][nextCol]) // Particular case with a merge
                    {
                        // If it's a "new" merge, ... merge the tiles
                        this->setBoard(nextRow, nextCol, this->getBoard(nextRow, nextCol)*2); // board[nextRow][nextCol] *= 2;
                        this->setBoard(row, col, 0); // board[row][col] = 0;
                        score += this->getBoard(nextRow, nextCol); // score += board[nextRow][nextCol];

                        // Check win condition
                        if(this->getBoard(nextRow, nextCol) == 2048)
                        {
                            std::cout << "You won !!! Congratulations !!!" << std::endl;
                            std::cout << "Your final score is : " << score << std::endl;
                            win = true;
                            return false;
                        }

                        // Update the new tile as "already merged" this turn
                        mergedTiles[nextRow][nextCol] = true;

                        // Indicate that a move had be done
                        movePossible = true;
                        canAddTile = true;
                        //std::cout << "Merged :  (" << nextRow << ";" << nextCol << ")" << std::endl;
                    }
                } // move possible ?
            } // columns
        } // rows
    } while (movePossible);

    // When all moves had be done this turn, reset tiles "already merged"
    resetTilesMoved();

    // Checking if a move had be done
    if(canAddTile)
    {
        // If we can't add a new tile, it's lose condition
        if(!generateNewTile())
        {
            std::cout << "You lose !" << std::endl;
            lost = true;
            return false;
        }
        return true;
    }
    return false;
}

/**
 * move : Call applyMove if user's input is correct (Z,Q,S,D or L(to leave)) and check for win or lose
 * @param command : int corresponding to User input (ASCII code)
 * @return true if the move was possible or impossible (just printing "impossible move" and trying another turn)
 *         false if user lost or won
 */
bool game2048::move(int command)
{
    int direction = -1;
    //      0
    //    3   1
    //      2
    switch (command)
    {
        case 90:       // Z or z, go Up
        case 122:
            direction = 0;
            break;
        case 68:       // D or d, go Right
        case 100:
            direction = 1;
            break;
        case 83:        // S or s, go Down
        case 115:
            direction = 2;
            break;
        case 81:        // Q or q, go Left
        case 113:
            direction = 3;
            break;
        default:
            std::cout << "Please enter correct key (Z,Q,S,D or L to leave)" << std::endl;
    }

    // Next thing is awful (re-checking losing and winning condition)
    if(!applyMove(direction))
    {
        if(lost || win)
            return false;
        else
            std::cout << "This move is impossible" << std::endl;
    }
    return true;
}
