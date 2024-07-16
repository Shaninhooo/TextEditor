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
                            if (totalLength > currentIndex + X) {
                                break;
                            }
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

    for(auto piece:Pieces){
        std::cout << piece.bufferType << " " << piece.startIndex << " " << piece.Length << " " << piece.lineNum << " " << std::endl;
    }
    std::cout << std::endl;
}

void PieceTable::deleteText(int X, int Y) {
     if (!rateLimiterDelete.canCall()) {
        std::cerr << "deleteText call blocked due to rate limiting." << std::endl;
        return;
    }
    bool deletionStarted = false;
    int totalLength = 0;
    int lineTextTotal = 0;

    for(auto piece:Pieces) {
        if(piece.lineNum == Y) {
            lineTextTotal += piece.Length;
        }
    }

    for (auto it = Pieces.begin(); it != Pieces.end(); it++) {
        if (it->lineNum == Y) {
        
            totalLength += it->Length;
            if (lineTextTotal > X && totalLength >= X) {
                int currentIndex = std::distance(Pieces.begin(), it);
                if(it->bufferType == "original") {
                    Piece newPiece1("original", it->startIndex, X - 1, it->lineNum);
                    Piece newPiece2("original", it->startIndex + X, it->Length - X, it->lineNum);
    
                    // Erase the original "add" piece
                    it = Pieces.erase(it);

                    // Insert new pieces in correct positions
                    it = Pieces.insert(Pieces.begin() + currentIndex, newPiece1);
                    it = Pieces.insert(Pieces.begin() + currentIndex + 1, newPiece2);
                } else if (it->bufferType == "add") {
                    if(it->Length > 1) {
                        Piece newPiece1("add", it->startIndex, X - 1, it->lineNum);
                        Piece newPiece2("add", it->startIndex + X, it->Length - X, it->lineNum);
        
                        // Erase the original "add" piece
                        it = Pieces.erase(it);

                        // Insert new pieces in correct positions
                        it = Pieces.insert(Pieces.begin() + currentIndex, newPiece1);
                        it = Pieces.insert(Pieces.begin() + currentIndex + 1, newPiece2);
                    } else {
                        it = Pieces.erase(it);
                    }
                }
                undoStack.push(Action(DELETE, currentIndex+1, 1, it->bufferType, X-1));

                deletionStarted = true;
                break;
            } else if (totalLength >= X) {

                // Handle partial deletion within a piece
                it->Length -= 1;
                if(it->Length == 0) {
                    Pieces.erase(it);
                }
                int pieceIndex = Pieces.size();
                std::cout << pieceIndex << std::endl;
                undoStack.push(Action(DELETE, pieceIndex, 1, it->bufferType, it->startIndex + it->Length));

                deletionStarted = true;
                break;
            } 
        }
    }

    if (!deletionStarted) {
        std::cerr << "Error: Deletion Canceled." << std::endl;
        std::cout << totalLength << " " << X << std::endl;
    }
}

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

void PieceTable::Undo() {
    if (undoStack.empty()) {
        std::cerr << "No actions to undo." << std::endl;
        return;
    }
    Action lastAction = undoStack.top();
    undoStack.pop();

    if (lastAction.type == INSERT) {
        Pieces.erase(Pieces.begin() + lastAction.pieceIndex);
    } else if (lastAction.type == DELETE) {
        Pieces.insert(Pieces.begin() + lastAction.pieceIndex, Piece(lastAction.bufferType, lastAction.bufferStartIndex, lastAction.length, 0)); // Adjust lineNum as needed
        // Adjust the pieceIndex of remaining actions in the undo stack
        std::stack<Action> tempStack;
        while (!undoStack.empty()) {
            Action action = undoStack.top();
            undoStack.pop();

            if (action.pieceIndex >= lastAction.pieceIndex) {
                action.pieceIndex++;
            }

            tempStack.push(action);
        }

        // Push adjusted actions back to the undo stack
        while (!tempStack.empty()) {
            undoStack.push(tempStack.top());
            tempStack.pop();
        }
    }
}