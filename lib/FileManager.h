#ifndef FileManager_H
#define FileManager_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <SDL.h>

class FileManager {
    public:
        void saveTextToFile(std::string filename, std::string text);
        bool openTextFromFile(std::string filename, std::string text);
};

#endif // FileManager_H