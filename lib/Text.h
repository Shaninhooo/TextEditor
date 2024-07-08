#ifndef TEXT_H
#define TEXT_H

#include <string.h>
#include "PieceTable.h"

class Text {
    private:
        std::string originalBuffer;
        std::string addBuffer;
        PieceTable piecetable;
    public:
        friend class PieceTable;
        
};


#endif