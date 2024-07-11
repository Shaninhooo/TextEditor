#ifndef PIECETABLE_H
#define PIECETABLE_H
#include <string>
#include <vector>
#include <iostream>
#include <stack>
#include <sstream>
#include <map>

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
};

class PieceTable {
    private:
        std::string originalBuffer = "Welcome To Text Editor";
        std::string addBuffer = "";
        std::vector<Piece> Pieces;
        std::stack<Action> undoStack;
        std::stack<Action> redoStack;
    public:
        PieceTable();
        void addRow(std::string bufferType, int startIndex, int Length, int lineNum);
        void addRowAtIndex(std::string bufferType, int startIndex, int Length, int lineNum, int index);
        void appendText(const std::string& text, int X, int Y);
        void deleteText();
        std::string getSequence();
        std::map<int, std::string> getLines();
};

#endif // PIECETABLE_H