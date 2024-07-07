#include "../lib/PieceTable.h"

void PieceTable::addRow(std::string Which, int startIndex, int Length) {
    this.Which.push_back(Which);
    this.startIndex.push_back(startIndex);
    this.Length.push_back(Length);
};