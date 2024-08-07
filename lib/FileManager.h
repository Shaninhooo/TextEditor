#ifndef FileManager_H
#define FileManager_H
#include <fstream>
#include <SDL.h>
#include <filesystem>
#include <cstdlib>

class FileManager {
    public:
        void saveTextToFile(std::string filename, std::string text);
        std::string getDownloadsFolder();
        bool openTextFromFile(std::string filename, std::string text);
};

#endif // FileManager_H