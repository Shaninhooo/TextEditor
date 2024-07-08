#include "../lib/PieceTable.h"

PieceTable::PieceTable() {
    Pieces.emplace_back("original", 0, originalBuffer.length());
};

void PieceTable::addRow(std::string bufferType, int startIndex, int Length) {
    Pieces.emplace_back(bufferType, startIndex, Length);
};

void PieceTable::appendText(const std::string& text) {
    addBuffer += text;

    int startIndex = addBuffer.size();

    if (!Pieces.empty() && Pieces.back().bufferType == "add") {
        Pieces.back().Length += text.size();
    } else {
        // Otherwise, add a new piece for the add buffer
        addRow("add", startIndex, text.size());
    }

    std::cout << text << std::endl;
};

void PieceTable::deleteText() {
    // addBuffer.push_back();
    // pieceTable.
};

std::string PieceTable::getOriginalBuffer() {
    return originalBuffer;
};

 std::string PieceTable::getSequence() {
    std::string sequence = originalBuffer;
    for (auto piece : Pieces) {
        if (piece.bufferType == "add") {
            sequence += addBuffer.substr(piece.startIndex, piece.Length);
        }
    }

    return sequence;
 };


// PieceTable::~PieceTable() {
//     for(auto Piece:Pieces) {
//         delete Piece;
//     }
// }