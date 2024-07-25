#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include "lib/PieceTable.h"

const int MAX_TEXT_LENGTH = 256;
const int WIDTH = 800;
const int HEIGHT = 600;
const int LINE_HEIGHT = 24;

RateLimiter rateLimiterType{1000};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

PieceTable piecetable;

std::map<int, std::string> textToRender = piecetable.getLines();

int cursorX = textToRender[0].size();
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

    for (auto line:textToRender) {
        int y = line.first * LINE_HEIGHT;
        if (y >= 0 && y < HEIGHT) {
            renderText(line.second, 0, y);
        }
    }

    // Calculate cursor position in pixels
    int cursorPosX = 0;
    if (cursorX > 0) {
        std::string substring = textToRender[cursorY].substr(0, cursorX);
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

void handleEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_BACKSPACE:
                {
                    // Find the max element in the map by getting the last key
                    int maxElement = textToRender.rbegin()->first;
                    if(textToRender.size()-1 < cursorY && cursorY > 0 && cursorX == 0) {
                        std::cout << "Moving Up Line" << std::endl;
                        cursorX = textToRender[textToRender.size()-1].size();
                        cursorY = textToRender.size()-1;
                    
                    } else if (cursorY <= maxElement && cursorX > 0) {
                        piecetable.deleteText(cursorX, cursorY);
                        textToRender = piecetable.getLines();
                        cursorX--;
                    }
                    break;
                }
                case SDLK_UP:
                    if(cursorY > 0) {
                        cursorY--;
                        if(textToRender.size() > cursorY) {
                            cursorX = textToRender[cursorY].size();
                        } else {
                            cursorX = 0;
                        }
                    }
                    break;
                case SDLK_DOWN:
                    cursorY++;
                    cursorX = 0;
                    break;
                case SDLK_LEFT:
                    if(cursorX > 0) {
                        cursorX--;
                    }
                    break;
                case SDLK_RIGHT:
                    if(textToRender.size() > cursorY) {
                            if(textToRender[cursorY].size() > cursorX) {
                                cursorX++;
                            }
                        }
                    
                    break;
                case SDLK_z:
                    if(SDL_GetModState() & KMOD_CTRL) {
                        if(!piecetable.getUndoStack().empty()) {
                            piecetable.Undo();
                            textToRender = piecetable.getLines();
                            if(piecetable.getUndoStack().top().type == INSERT) {
                                cursorX--;
                            }  else {
                                cursorX++;
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        case SDL_TEXTINPUT:
            if (!rateLimiterType.canCall()) {
                std::cerr << "deleteText call blocked due to rate limiting." << std::endl;
                return;
            }
            piecetable.appendText(event.text.text, cursorX, cursorY); // Append the new text
            textToRender = piecetable.getLines();
            cursorX++;
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

        render();
    }

    close();

    return 0;
}