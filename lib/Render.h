
#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Render {
    public:
        SDL_Renderer* renderer = nullptr;
        TTF_Font* font = nullptr;
        void renderCursor(int x, int y); 
        void renderText(const std::string& text, int x, int y);
        void render(const std::string& text, int cursorX, int cursorY, bool showCursor);
};

#endif