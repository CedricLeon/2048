#ifndef INC_2048_GAME2048_H
#define INC_2048_GAME2048_H

#include <iostream>
#include <cstdlib>
#include <ctime>

#include <gegelati.h>

class game2048 : public Learn::LearningEnvironment
{
private:

    // ----- Constant -----
    static const size_t REWARD_HISTORY_SIZE = 300;

    // Reward history for score computation
    double rewardHistory[REWARD_HISTORY_SIZE];

    // Randomness control
    Mutator::RNG rng;

    /**
    * \brief Available actions for the LearningAgent.
    *
    * Each number $a$ in this list, corresponds to one
    * action available for the LearningAgent : Going Up (Z : 90), Down (S : 83), Right (D : 68) or Left (Q : 81)
    *
    * ASCII code is used
    */
    const std::vector<uint64_t> availableActions;


    // Current board containing 0 as empty or the tile value (2, 4, 8, etc...), size is 4*4 (row-order)
    // 0  1  2  3
    // 4  5  6  7
    // 8  9  10 11
    // 12 13 14 15
    // board[i][j] <=> 4*i + j
    Data::PrimitiveTypeArray<int> board; // Used as current state

    // ----- Variables -----
    bool mergedTiles[4][4];     // Another board where each turn we will update if a tile has already be merged
    bool lost = false;          // Boolean indicating the lost
    bool win  = false;          // Boolean indicating the win
    int score = 0;              // Total score of the game

protected:
    // Setter for a new state (updating board)
    void setBoard(int i, int j, int newValue);
    int getBoard(int i, int j);

public:
    game2048(std::vector<uint64_t> actions) : LearningEnvironment(4), board(16), availableActions(actions)
    {
        // Call resetBoard()
        for(int row = 0; row < 4; ++row)
            for(int col = 0; col < 4; ++col)
                this->setBoard(row, col, 0); // board[row][col] = 0;
        // 2 random Tiles start with a 2 or a 4
        for(int i = 0; i < 2; ++i)
            generateNewTile();
    }

    // Print functions : used when a Human play (2nd main)
    static void printTile(int value) ;
    void printUI();

    // Initialisation
    std::pair<int, int> generateUnoccupiedTile();
    int generateValueNewTile();
    bool generateNewTile();
    void initGame();
    void resetTilesMoved();

    // Move
    bool moveIsPossible(int row, int col, int nextRow, int nextCol);
    bool applyMove(int direction);
    bool move(int command);

    // Redéfinition des virtual de LearningEnvironment : (override ?)
    LearningEnvironment* clone() const;
    bool isCopyable() const;
    void doAction(uint64_t actionID);
    void reset(size_t seed = 0, Learn::LearningMode mode = Learn::TRAINING);
    std::vector<std::reference_wrapper<const Data::DataHandler>> getDataSources();
    double getScore() const;
    bool isTerminal() const;
};
#endif //INC_2048_GAME2048_H
