#ifndef PIECETABLE_H
#define PIECETABLE_H
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <map>
#include "RateLimiter.h"

struct Piece {
    std::string bufferType;
    int startIndex;
    int Length;
    int lineNum;

    Piece(std::string bufferType, int startIndex, int length, int lineNum)
        : bufferType(bufferType), startIndex(startIndex), Length(length), lineNum(lineNum) {}
};

enum ActionType { INSERT, DELETE };

struct Action {
    ActionType type;
    int pieceIndex;
    int length;
    std::string bufferType;
    int bufferStartIndex;
     Action(ActionType t, int idx, int len, std::string bType, int bStart)
        : type(t), pieceIndex(idx), length(len), bufferType(bType), bufferStartIndex(bStart) {}
};

class PieceTable {
    private:
        std::string originalBuffer = "Welcome To Text Editor";
        std::string addBuffer = "";
        std::vector<Piece> Pieces;
        std::stack<Action> undoStack;
        std::stack<Action> redoStack;
        RateLimiter rateLimiterDelete{1000}; // 1 second timeout for delete
        RateLimiter rateLimiterUndo{1000}; // 1 second timeout for undo
    public:
        PieceTable();
        void addRow(std::string bufferType, int startIndex, int Length, int lineNum);
        void addRowAtIndex(std::string bufferType, int startIndex, int Length, int lineNum, int index);
        void appendText(const std::string& text, int X, int Y);
        void deleteText(int X, int Y);
        std::string getSequence();
        std::map<int, std::string> getLines();
        void Undo();
};

#endif // PIECETABLE_H