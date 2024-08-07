#include "../lib/FileManager.h"

std::string FileManager::getDownloadsFolder() {
    #ifdef _WIN32
        char* userProfile = std::getenv("USERPROFILE");
        if (userProfile) {
            return std::string(userProfile) + "\\Downloads";
        }
    #elif __APPLE__ || __linux__
        char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + "/Downloads";
        }
    #endif
    return "./"; // Fallback to current directory
}

void FileManager::saveTextToFile(std::string filename, std::string text) {
    std::string downloadsFolder = getDownloadsFolder();
    std::filesystem::path filePath = std::filesystem::path(downloadsFolder) / filename;

    // Ensure the directory exists
    std::filesystem::path dir = filePath.parent_path();
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }

    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << text;
        outFile.close();
        SDL_Log("File written and closed successfully: %s", filePath.string().c_str());
    } else {
        SDL_Log("Unable to open file for writing: %s", filePath.string().c_str());
    }
}

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