#include "../lib/PieceTable.h"

PieceTable::PieceTable() {
    Pieces.emplace_back("original", 0, originalBuffer.length(), 0);
};

void PieceTable::addRow(std::string bufferType, int startIndex, int Length, int lineNum) {
    Pieces.emplace_back(bufferType, startIndex, Length, lineNum);
};


// Append Text Needs Rework
void PieceTable::appendText(const std::string& text, int X, int Y) {
    int startIndex = addBuffer.size();
    addBuffer += text;
    std::cout << X << " " << Y << std::endl;

    // Iterate through pieces to find the correct line and position to insert text
    int currentLine = 0;
    for (auto it = Pieces.begin(); it != Pieces.end(); ++it) {
        if (it->lineNum == Y) {
            int pieceStart = (it->bufferType == "original") ? it->startIndex : it->startIndex + originalBuffer.size();
            int pieceEnd = pieceStart + it->Length;

            // Insert text at the correct position within the line
            if (X <= pieceEnd) {
                int splitIndex = X - pieceStart;
                
                // Split the piece if necessary and insert the new piece for the added text
                if (splitIndex > 0) {
                    addRow(it->bufferType, it->startIndex, splitIndex, it->lineNum);
                }
                addRow("add", startIndex, text.size(), it->lineNum);
                if (splitIndex < it->Length) {
                    addRow(it->bufferType, it->startIndex + splitIndex, it->Length - splitIndex, it->lineNum);
                }

                // Remove the original piece since it has been split and replaced
                it = Pieces.erase(it); // Update the iterator after erase
                break;
            }
        }
    }
}


void PieceTable::deleteText() {
    // addBuffer.push_back();
    // pieceTable.
};

std::string PieceTable::getSequence() {
    std::string sequence;
    for (auto piece : Pieces) {
        std::cout << piece.bufferType << piece.startIndex << piece.Length << piece.lineNum << std::endl;  
        if (piece.bufferType == "original") {
            sequence += originalBuffer.substr(piece.startIndex, piece.Length);
        } else {
            sequence += addBuffer.substr(piece.startIndex, piece.Length);
        }
        if (piece.Length > 0 && sequence.back() != '\n') {
            sequence += '\n';
        }
    }
    std::cout << sequence << std::endl;
    return sequence;
}

std::vector<std::string> PieceTable::getLines() {
    std::vector<std::string> lines;
    std::string sequence = getSequence();
    std::istringstream stream(sequence);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

// PieceTable::~PieceTable() {
//     for(auto Piece:Pieces) {
//         delete Piece;
//     }
// }