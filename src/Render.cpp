#include "../lib/Render.h"

const int MAX_TEXT_LENGTH = 256;
const int WIDTH = 800;
const int HEIGHT = 600;
const int LINE_HEIGHT = 24;

int cursorX = 0;
int cursorY = 0;
int scrollOffset = 0;
bool showCursor = true;  // Cursor visibility for blinking
Uint32 lastBlinkTime = 0;


void Render::renderText(const std::string& text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Render::renderCursor(int x, int y) {
    if (showCursor) {
        SDL_Rect cursorRect = {x, y, 2, LINE_HEIGHT};  // Width of 2 pixels
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White color
        SDL_RenderFillRect(renderer, &cursorRect);
    }
}

void Render::render(std::string text, int cursorX, int cursorY, bool showCursor) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < 10; ++i) {
        int y = i * LINE_HEIGHT;
        if (y >= 0 && y < HEIGHT) {
            renderText(text, 0, y);
        }
    }

    // Calculate cursor position in pixels
    int cursorPosX = 0;
    if (cursorX > 0) {
        std::string substring = text.substr(0, cursorX);
        SDL_Surface* surface = TTF_RenderText_Solid(font, substring.c_str(), {255, 255, 255, 255});
        cursorPosX = surface->w;
        SDL_FreeSurface(surface);
    }
    int cursorPosY = cursorY * LINE_HEIGHT;

    if (showCursor) {
        renderCursor(cursorPosX, cursorPosY);
    }

    SDL_RenderPresent(renderer);
}