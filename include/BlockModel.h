#pragma once

#include <vector>

const char DEFAULT_ICON = '.';
const char DEFAULT_EMPTY = '.';
const char DEFAULT_APPLE = '@';
const char DEFAULT_SNAKE = '#';

struct BlockPosition {
    int row;
    int col;

    BlockPosition nextPositionOfDirection(const std::vector<int> &change) {
        return BlockPosition{row + change[0], col + change[1]};
    }
};

class BlockBase {
public:
    BlockBase() {
        m_row = m_col = 0;
        m_icon = DEFAULT_ICON;
    }

    BlockBase(int row, int col, char icon) {
        this->m_row = row;
        this->m_col = col;
        this->m_icon = icon;
    }

    int getCol() { return m_col; }
    int getRow() { return m_row; }

    char getIcon() { return m_icon; }

    void setPosition(int row, int col) {
        m_row = row;
        m_col = col;
    }

    void setPosition(const BlockPosition pos) {
        m_row = pos.row;
        m_col = pos.col;
    }

    BlockPosition getPosition() {
        return BlockPosition{m_row, m_col};
    }

protected:
    int m_row;
    int m_col;
    char m_icon;
};

class EmptyBlock : public BlockBase {
public:
    EmptyBlock() {
        this->m_row = 0;
        this->m_col = 0;
        this->m_icon = DEFAULT_EMPTY;
    }

    EmptyBlock(const BlockPosition pos) {
        this->m_row = pos.row;
        this->m_col = pos.col;
        this->m_icon = DEFAULT_EMPTY;
    }

    EmptyBlock(const int row, const int col) {
        this->m_row = row;
        this->m_col = col;
        this->m_icon = DEFAULT_EMPTY;
    }
};

class SnakeBlock : public BlockBase {
public:
    SnakeBlock() {
        this->m_row = 0;
        this->m_col = 0;
        this->m_icon = DEFAULT_SNAKE;
    }

    SnakeBlock(const BlockPosition pos) {
        this->m_row = pos.row;
        this->m_col = pos.col;
        this->m_icon = DEFAULT_SNAKE;
    }

    SnakeBlock(const int row, const int col) {
        this->m_row = row;
        this->m_col = col;
        this->m_icon = DEFAULT_SNAKE;
    }
};

class AppleBlock : public BlockBase {
public:
    AppleBlock() {
        this->m_row = 0;
        this->m_col = 0;
        this->m_icon = DEFAULT_APPLE;
    }

    AppleBlock(const BlockPosition pos) {
        this->m_row = pos.row;
        this->m_col = pos.col;
        this->m_icon = DEFAULT_APPLE;
    }

    AppleBlock(const int row, const int col) {
        this->m_row = row;
        this->m_col = col;
        this->m_icon = DEFAULT_APPLE;
    }
};
