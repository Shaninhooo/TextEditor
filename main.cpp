#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

const int MAX_TEXT_LENGTH = 256;
const int WIDTH = 800;
const int HEIGHT = 600;
const int LINE_HEIGHT = 24;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (renderer == nullptr) {
        std::cerr << "renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    // Load font
    font = TTF_OpenFont("fonts/BRLNSB.TTF", 24);
    if (font == nullptr) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

void close() {
    SDL_StopTextInput();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
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

void renderText(const std::string& text, int x, int y) {
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderCursor(int x, int y) {
    if (showCursor) {
        SDL_Rect cursorRect = {x, y, 2, LINE_HEIGHT};  // Width of 2 pixels
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White color
        SDL_RenderFillRect(renderer, &cursorRect);
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < 10; ++i) {
        int y = i * LINE_HEIGHT;
        if (y >= 0 && y < HEIGHT) {
            renderText(textToRender, 0, y);
        }
    }

    // Calculate cursor position in pixels
    int cursorPosX = 0;
    if (cursorX > 0) {
        std::string substring = textToRender.substr(0, cursorX);
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

        render();
    }

    close();

    return 0;
}