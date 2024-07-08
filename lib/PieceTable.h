#ifndef PIECETABLE_H
#define PIECETABLE_H
#include <string>
#include <vector>
#include <iostream>

class PieceTable {
    private:
        std::string originalBuffer = "asdasdasd";
        std::string addBuffer = " ";
 
        struct Piece {
            std::string bufferType;
            int startIndex;
            int Length;

            Piece(std::string bufferType, int startIndex, int length)
                : bufferType(bufferType), startIndex(startIndex), Length(length) {}
        };

        std::vector<Piece> Pieces;
    public:
        PieceTable();
        void addRow(std::string bufferType, int startIndex, int Length);
        void appendText(const std::string& text);
        void deleteText();
        std::string getOriginalBuffer(); 
        std::string getSequence();
};

#endif // PIECETABLE_H