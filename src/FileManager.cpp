#include "../lib/FileManager.h"

void FileManager::saveTextToFile(std::string filename, std::string text) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << text;
        outFile.close();
    } else {
        SDL_Log("Unable to open file for writing: %s", filename.c_str());
    }
    std::cout << "File Saved" << std::endl;
};


// bool FileManager::openTextFromFile(std::string filename, std::string text) {
//     std::ifstream inFile(filename);
//     if (inFile.is_open()) {
//         std::stringstream buffer;
//         buffer << inFile.rdbuf();
//         textBuffer = buffer.str();
//         inFile.close();
//         return true;
//     } else {
//         SDL_Log("Unable to open file for reading: %s", filename.c_str());
//         return false;
//     }
// };