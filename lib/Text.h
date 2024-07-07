#ifndef TEXT_H
#define TEXT_H

#include <string.h>
#include "PieceTable.h"

class Text {
    private:
        std::string Sequence;
        std::string originalBuffer;
        std::string addBuffer;
        PieceTable piecetable;
    public:
        friend class PieceTable;
        void appendText(std::string text);
        void deleteText();
        std::string getSequence(); 
        std::string getOriginalBuffer(); 
};


#endif