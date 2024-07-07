#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "lib/Render.h"

const int MAX_TEXT_LENGTH = 256;
const int WIDTH = 800;
const int HEIGHT = 600;
const int LINE_HEIGHT = 24;

SDL_Window* window = nullptr;
Render render;
SDL_Renderer* Renderer = render.renderer;
TTF_Font* Font = render.font;

std::string textToRender = " ";
int cursorX = 0;
int cursorY = 0;
int scrollOffset = 0;
bool showCursor = true;  // Cursor visibility for blinking
Uint32 lastBlinkTime = 0;

bool Init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("SDL Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (Renderer == nullptr) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Load font
    Font = TTF_OpenFont("fonts/BRLNSB.TTF", 24);
    if (Font == nullptr) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(Renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

void close() {
    SDL_StopTextInput();
    TTF_CloseFont(Font);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void handleEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_BACKSPACE && !textToRender.empty()) {
                textToRender.pop_back();
            }
            break;
        case SDL_TEXTINPUT:
            textToRender += event.text.text; // Append the new text
            break;
        default:
            break;
    }
}

int main(int argc, char* argv[]) {
    if (!Init()) {
        return -1;
    }

    SDL_Color textColor = {255, 255, 255, 255}; // White color

    bool running = true;
    SDL_Event event;
    SDL_StartTextInput();

    // Main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else {
                handleEvent(event);
            }
        }

        render.render(textToRender, cursorX, cursorY, showCursor);
    }

    close();

    return 0;
}
