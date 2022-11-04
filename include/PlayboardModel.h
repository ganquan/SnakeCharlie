#pragma once

#include "BlockModel.h"
#include "Observer/Observer.h"
#include "Utility.h"

class SnakeModel;
class SnakeBrain;

class PlayboardModel : public Observable<PlayboardModel>,
                       public Observer<SnakeModel> {

    friend SnakeBrain;
public:
    PlayboardModel();
    PlayboardModel(int row, int col);
    ~PlayboardModel();

    void initPlayboard();
    void update();

    void placeApple();

    inline bool isApple(const int row, const int col) { return (m_board[row][col] == DEFAULT_APPLE); }
    inline bool isSnake(const int row, const int col) { return (m_board[row][col] == DEFAULT_SNAKE); }
    inline bool isEmpty(const int row, const int col) { return (m_board[row][col] == DEFAULT_EMPTY); }

    void reset();

public:
    int getRow() { return m_row; }
    int getCol() { return m_col; }
    BlockPosition getApplePosition() { return m_applePosition; }

private:
    // Observer
    void field_changed(SnakeModel &source, const string &field_name);

private:
    void eraseApple();

    void addBlock(BlockBase drawable) { addBlockImpl(drawable.getRow(), drawable.getCol(), drawable.getIcon()); }
    void addBlockImpl(int y, int x, char ch) { m_board[y][x] = ch; }

    void getRandomEmptyPosition(int &row, int &col) {
        while (row = utility::random::generateRandomNumber(0, m_row - 1),
               col = utility::random::generateRandomNumber(0, m_col - 1), !isEmpty(row, col))
            ;
    }

    bool isWithinBound(BlockPosition pos) {
        if (pos.row < 0 || pos.row >= getRow()) {
            return false;
        }
        if (pos.col < 0 || pos.col >= getCol()) {
            return false;
        }

        return true;
    }

private:
    int m_row;
    int m_col;

    char **m_board;

    BlockPosition m_applePosition;
};