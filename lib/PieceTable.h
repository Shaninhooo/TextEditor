#ifndef PIECETABLE_H
#define PIECETABLE_H
#include <string.h>
#include <vector>

class PieceTable {
    private:
        std::vector<std::string> Which;
        std::vector<int> startIndex;
        std::vector<int> Length;
    public:
        void addRow(std::string Which, int startIndex, int Length);
};

#endif