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


// Append Text Is Good I think So Far...
void PieceTable::appendText(const std::string& text, int X, int Y) {
    if (!rateLimiterDelete.canCall()) {
        std::cerr << "deleteText call blocked due to rate limiting." << std::endl;
        return;
    }
    int startIndex = addBuffer.size();
    addBuffer += text;
    bool inserted = false;
    int lineTextTotal = 0;
    int pieceIndex = 0;
    int currentTotalText = 0;

    for(auto piece:Pieces) {
        if(piece.lineNum == Y) {
            lineTextTotal += piece.Length;
        }
    }

    // Iterate through pieces to find the correct line and position to insert text
    int currentLine = 0;

    for (auto it = Pieces.begin(); it != Pieces.end(); ++it) {
        
        if (it->lineNum == Y) {

            currentTotalText += it->Length;
            int currentIndex = std::distance(Pieces.begin(), it);

            if (X <= lineTextTotal && currentTotalText >= X) {
                int pieceX = X - (currentTotalText - it->Length);

                // Splitting the "add" piece into three parts
                Piece newPiece1(it->bufferType, it->startIndex, pieceX, it->lineNum);
                Piece newTextPiece("add", startIndex, text.size(), Y);
                Piece newPiece2(it->bufferType, it->startIndex + pieceX, it->Length - pieceX, it->lineNum);

                std::cout << pieceX << " " << it->Length << std::endl;
                if(pieceX == it->Length) {
                    std::cout << "Inserting Letter at end of piece" << std::endl;
                    Pieces.insert(it + 1, newTextPiece);
                } else {
                    std::cout << "Inserting Letter inbetween of piece" << std::endl;
                    // Erase the original "add" piece
                    it = Pieces.erase(it);

                    // Insert new pieces in correct positions
                    if (newPiece1.Length > 0) {
                        it = Pieces.insert(it, newPiece1);
                        it++; // Adjust iterator
                    }
                    if (newTextPiece.Length > 0) {
                        it = Pieces.insert(it, newTextPiece);
                        it++; // Adjust iterator
                    }
                    if (newPiece2.Length > 0) {
                        Pieces.insert(it, newPiece2);
                    }

                }
            
                //Checks for sequences of text in buffer from pieces and combines if in sequence
                for (size_t i = 1; i < Pieces.size();) {
                    if (Pieces[i - 1].startIndex + Pieces[i - 1].Length == Pieces[i].startIndex &&  Pieces[i - 1].bufferType == Pieces[i].bufferType) {
                        combinePiece(i - 1, i);
                        // After combining, 'i' points to the next element, no need to increment 'i'
                    } else {
                        i++; // Increment only if no merge happened
                    }
                }
                pushUndo(INSERT, currentIndex + 1, 1, "add", startIndex);
                std::cout << addBuffer << " " << startIndex<< std::endl;
                inserted = true;
                break;
            }
        }
    }

    if (!inserted) {
        std::cerr << "Failed to insert the text at the specified position." << std::endl;
    } else {
        std::cout << "Piece Added" << std::endl;
    }
}



//Delete Text Needs Rework 
void PieceTable::deleteText(int X, int Y) {
    if (!rateLimiterDelete.canCall()) {
        std::cerr << "deleteText call blocked due to rate limiting." << std::endl;
        return;
    }
    bool deletionStarted = false;
    int totalLength = 0;
    int lineTextTotal = 0;

    // Check the total length of line Y
    for(auto piece:Pieces) {
        if(piece.lineNum == Y) {
            lineTextTotal += piece.Length;
        }
    }

    for (auto it = Pieces.begin(); it != Pieces.end(); it++) {
        if (it->lineNum == Y) {
            totalLength += it->Length;
            int pieceStart = totalLength - it->Length;

            if (lineTextTotal >= X && totalLength >= X) {
                int currentIndex = std::distance(Pieces.begin(), it);

                // Calculate the correct position within the piece
                int pieceX = X - pieceStart;
                std::string bufferType = it->bufferType; // Store the bufferType before any modifications
                int startIndex = it->startIndex; // Store startIndex before any modifications
                
                if(pieceX == 1) {
                    it->startIndex++;
                    it->Length--;

                }  else if(pieceX == it->Length) {
                    it->Length--;
                } else if(it->Length > 1) {
                    Piece newPiece1(it->bufferType, it->startIndex, pieceX - 1, it->lineNum);
                    Piece newPiece2(it->bufferType, it->startIndex + pieceX, it->Length - pieceX, it->lineNum);

                    // Erase the original "add" piece
                    it = Pieces.erase(it);

                    if (newPiece1.Length > 0) {
                        it = Pieces.insert(Pieces.begin() + currentIndex, newPiece1);
                        ++it;
                    }
                    if (newPiece2.Length > 0) {
                        it = Pieces.insert(Pieces.begin() + currentIndex + 1, newPiece2);
                    }
                } else {
                    Pieces.erase(it);
                }

                 deletionStarted = true;

                // Combine pieces if needed
                for (size_t i = 1; i < Pieces.size();) {
                    if (Pieces[i - 1].startIndex + Pieces[i - 1].Length == Pieces[i].startIndex && Pieces[i - 1].bufferType == Pieces[i].bufferType) {
                        combinePiece(i - 1, i);
                    } else {
                        ++i;
                    }
                }

                // Push undo action after combining pieces
                pushUndo(DELETE, currentIndex + 1, 1, bufferType, startIndex + pieceX - 1);
                
                break;
            } 
        }
    }

    

    if (!deletionStarted) {
        std::cerr << "Error: Deletion Canceled." << std::endl;
    }
}

void PieceTable::pushUndo(ActionType t, int idx, int len, std::string bType, int bStart) {
     // Adjust the pieceIndex of remaining actions in the undo stack

    std::stack<Action> tempStack;
    while (!undoStack.empty()) {
        Action action = undoStack.top();
        undoStack.pop();

        if (action.pieceIndex >= idx) {
            action.pieceIndex += 1;
        }
        tempStack.push(action);
    }

    // Push adjusted actions back to the undo stack
    while (!tempStack.empty()) {
        undoStack.push(tempStack.top());
        tempStack.pop();
        std::cout << undoStack.top().bufferType << " " << undoStack.top().pieceIndex << " " << undoStack.top().length << " " << undoStack.top().bufferStartIndex << " " << addBuffer[undoStack.top().bufferStartIndex] << std::endl;
    }
    undoStack.push(Action(t, idx, len, bType, bStart));
    std::cout << std::endl;
}

// Combining pieces cause undo to undo whole piece instead of a part of it...
void PieceTable::Undo() {
    if (undoStack.empty()) {
        std::cerr << "No actions to undo." << std::endl;
        return;
    }
    Action lastAction = undoStack.top();
    undoStack.pop();


    std::cout << lastAction.pieceIndex  << " " << Pieces.size() << std::endl;
    std::cout << std::endl;
    if (lastAction.pieceIndex < 0 || lastAction.pieceIndex > Pieces.size()) {
        std::cerr << "Error: Invalid piece index in undo stack." << std::endl;
        return;
    }

    if (lastAction.type == INSERT) {
        std::cout << "undoing INSERT" << std::endl;
        // Ensure the index is within bounds
        auto it = Pieces.begin() + lastAction.pieceIndex;
        Piece insertPiece = Pieces[lastAction.pieceIndex];
        if (lastAction.pieceIndex <= Pieces.size()) {
            std::cout << lastAction.bufferStartIndex << " " << insertPiece.Length << std::endl;

            if (lastAction.bufferStartIndex == 0 || insertPiece.Length == 1) {
                std::cout << "Remove start of Piece" << std::endl;
                insertPiece.startIndex++;
                insertPiece.Length--;
                if(insertPiece.Length == 0) {
                    Pieces.erase(it);
                } else {
                    *it = insertPiece;
                }   
            } else if (lastAction.bufferStartIndex == insertPiece.Length) {
                std::cout << "Shorten Piece" << " " << insertPiece.Length << std::endl;
                insertPiece.Length--;
                if(insertPiece.Length == 0) {
                    Pieces.erase(it);
                } else {
                    *it = insertPiece;
                }
            } else if (lastAction.bufferStartIndex < insertPiece.Length-1) {
                std::cout << "Del Inbetween Letter" << std::endl;
                int pieceX = lastAction.bufferStartIndex;
                Piece newPiece1(insertPiece.bufferType, insertPiece.startIndex, pieceX, insertPiece.lineNum);
                Piece newPiece2(insertPiece.bufferType, insertPiece.startIndex + pieceX + 1, insertPiece.Length - pieceX - 1, insertPiece.lineNum);

                // Erase the original "add" piece
                it = Pieces.erase(it);

                if (newPiece1.Length > 0) {
                    it = Pieces.insert(it, newPiece1);
                    ++it;
                }
                if (newPiece2.Length > 0) {
                    it = Pieces.insert(it, newPiece2);
                }
            }
        } else {
            std::cerr << "Error: Invalid piece index for erase." << std::endl;
        }
    } else if (lastAction.type == DELETE) {
        // Ensure the index is within bounds
        if (lastAction.pieceIndex <= Pieces.size()) {
            Pieces.insert(Pieces.begin() + lastAction.pieceIndex, Piece(lastAction.bufferType, lastAction.bufferStartIndex, lastAction.length, 0));
        } else {
            std::cerr << "Error: Invalid piece index for insert." << std::endl;
        }
    }

    // Optionally, you may add debug information to verify undo operation
    std::cout << std::endl;
}

void PieceTable::Redo() {
    
}


std::string PieceTable::getSequence() {
    std::string text = "";
    int lastLineNum = Pieces[0].lineNum;
    for(auto piece:Pieces) {
        if(piece.lineNum != lastLineNum) {
            text += "/n";
            if(piece.bufferType == "original") {
            std::cout << piece.lineNum << " " << originalBuffer.substr(piece.startIndex, piece.Length);
            text += originalBuffer.substr(piece.startIndex, piece.Length);
            } else {
                std::cout << piece.lineNum << " " << addBuffer.substr(piece.startIndex, piece.Length);
                text += addBuffer.substr(piece.startIndex, piece.Length);
            }
        } else {
            if(piece.bufferType == "original") {
            std::cout << piece.lineNum << " " << originalBuffer.substr(piece.startIndex, piece.Length);
            text += originalBuffer.substr(piece.startIndex, piece.Length);
            } else {
                std::cout << piece.lineNum << " " << addBuffer.substr(piece.startIndex, piece.Length);
                    text += addBuffer.substr(piece.startIndex, piece.Length);
            }
        }
        lastLineNum = piece.lineNum;
    }
    return text;
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



void PieceTable::combinePiece(int lastPieceIndex, int currentPieceIndex) {
    std::cout << "Combining pieces" << std::endl;
    Piece lastPiece = Pieces[lastPieceIndex];
    Piece currentPiece = Pieces[currentPieceIndex];

    // Ensure lastPieceIndex is less than currentPieceIndex
    if (lastPieceIndex > currentPieceIndex) {
        std::swap(lastPieceIndex, currentPieceIndex);
    }

    Pieces.erase(Pieces.begin() + currentPieceIndex);
    Pieces.erase(Pieces.begin() + lastPieceIndex);

    Piece combinedPiece(lastPiece.bufferType, lastPiece.startIndex, lastPiece.Length + currentPiece.Length, lastPiece.lineNum);
    Pieces.insert(Pieces.begin() + lastPieceIndex, combinedPiece);

    undoStack.top().pieceIndex = lastPieceIndex;
}

std::stack<Action> PieceTable::getUndoStack() {
    return undoStack;
}