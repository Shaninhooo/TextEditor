#include "../lib/PieceTable.h"

PieceTable::PieceTable() {
    Pieces.emplace_back("original", 0, originalBuffer.length(), 0);
};

void PieceTable::addRow(std::string bufferType, int startIndex, int Length, int lineNum) {
    Pieces.emplace_back(bufferType, startIndex, Length, lineNum);
};

void PieceTable::addRowAtIndex(std::string bufferType, int startIndex, int Length, int lineNum, int index) {
    Piece newPiece(bufferType, startIndex, Length, lineNum);
    auto it = Pieces.begin() + index;
    Pieces.insert(it, newPiece);
};


// Append Text In Between Needs Rework
void PieceTable::appendText(const std::string& text, int X, int Y) {
    int startIndex = addBuffer.size();
    addBuffer += text;
    bool inserted = false;
    int lineTextTotal = 0;
    for(auto piece:Pieces) {
        if(piece.lineNum == Y) {
            lineTextTotal += piece.Length;
        }
    }

    // Iterate through pieces to find the correct line and position to insert text
    int currentLine = 0;

    for (auto it = Pieces.begin(); it != Pieces.end(); ++it) {
        if (it->lineNum == Y) {
            if (it->bufferType == "add" && X == lineTextTotal) {
                it->Length += text.size();
                inserted = true;
                break;
            
            } else if (X < lineTextTotal) {
                if(it->bufferType == "original") {
                    int currentIndex = std::distance(Pieces.begin(), it);

                    if (X > it->Length) {
                        Piece newTextPiece("add", startIndex, text.size(), Y);
                        // Insert new pieces in correct positions

                        int pieceIndex = 0;
                        int totalLength = 0;
                        while(totalLength != currentIndex + X) {
                            totalLength += Pieces[pieceIndex].Length;
                            pieceIndex++;
                        }
                        Pieces.insert(Pieces.begin() + pieceIndex, newTextPiece);

                        inserted = true;
                        break;
                    }

                    // Splitting the "add" piece into three parts
                    Piece newPiece1("original", it->startIndex, X, it->lineNum);
                    Piece newTextPiece("add", startIndex, text.size(), Y);
                    Piece newPiece2("original", it->startIndex + X, it->Length - X, it->lineNum);

                    // Erase the original "add" piece
                    it = Pieces.erase(it);

                    // Insert new pieces in correct positions
                    it = Pieces.insert(Pieces.begin() + currentIndex, newPiece1);
                    it = Pieces.insert(Pieces.begin() + currentIndex + 1, newTextPiece);
                    Pieces.insert(Pieces.begin() + currentIndex + 2, newPiece2);

                    inserted = true;
                    break;
                } else {

                    int currentIndex = std::distance(Pieces.begin(), it);

                    if (X > it->Length) {
                        Piece newTextPiece("add", startIndex, text.size(), Y);
                        // Insert new pieces in correct positions
                        Pieces.insert(Pieces.begin() + currentIndex + X, newTextPiece);

                        inserted = true;
                        break;
                    }

                    // Splitting the "add" piece into three parts
                    Piece newPiece1("add", it->startIndex, X, it->lineNum);
                    Piece newTextPiece("add", startIndex, text.size(), Y);
                    Piece newPiece2("add", it->startIndex + X, it->Length - X, it->lineNum);

                    // Erase the original "add" piece
                    it = Pieces.erase(it);

                    // Insert new pieces in correct positions
                    it = Pieces.insert(Pieces.begin() + currentIndex, newPiece1);
                    it = Pieces.insert(Pieces.begin() + currentIndex + 1, newTextPiece);
                    Pieces.insert(Pieces.begin() + currentIndex + 2, newPiece2);
        

                    inserted = true;
                    break;
                }
                break;
            }


        }
    }

    if (!inserted) {
        // Add new piece if it was not inserted in the loop
        addRow("add", startIndex, text.size(), Y);
    }

    // for(auto piece:Pieces){
    //     std::cout << piece.bufferType << " " << piece.startIndex << " " << piece.Length << " " << piece.lineNum << " " << std::endl;
    // }
    // std::cout << std::endl;
}

void PieceTable::deleteText() {
    // addBuffer.push_back();
    // pieceTable.
};

// std::string PieceTable::getSequence() {
//     std::string sequence;
//     for (auto piece : Pieces) {
//         std::cout << piece.bufferType << piece.startIndex << piece.Length << piece.lineNum << std::endl;  
//         if (piece.Length > 0 && sequence.back() != '\n') {
//             sequence += '\n';
//         }
//     }
//     std::cout << sequence << std::endl;
//     return sequence;
// }

std::map<int, std::string> PieceTable::getLines() {
    std::map<int, std::string> lines;
    for(auto piece:Pieces) {
        if (piece.bufferType == "original") {
            lines[piece.lineNum] += originalBuffer.substr(piece.startIndex, piece.Length);
        } else {
            lines[piece.lineNum] += addBuffer.substr(piece.startIndex, piece.Length);
        }
    }
    return lines;
}

// PieceTable::~PieceTable() {
//     for(auto Piece:Pieces) {
//         delete Piece;
//     }
// }