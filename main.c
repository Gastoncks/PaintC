#include "include/SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MY_FONT "C:\\Windows\\Fonts\\arial.ttf"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 1;
    }
    // Initialize SDL_ttf
    if (TTF_Init() < 0) {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    TTF_Font *font = TTF_OpenFont(MY_FONT, 16);
    if (font == NULL) {
        printf("Failed to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }        

    typedef struct {
        SDL_Window* window;
        SDL_Renderer* renderer;
        Uint32 windowID;
        SDL_Rect pen;
        int mouseX;
        int mouseY;
    } PaintWindow;

    PaintWindow win1;
    PaintWindow win2;

    unsigned int wightpen = 10;
    bool mousePressed = false;

    // Windows 1
    win1.window = SDL_CreateWindow(
        "GastonPaint",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN
    );
    win1.renderer = SDL_CreateRenderer(win1.window, -1, SDL_RENDERER_ACCELERATED);
    win1.windowID = SDL_GetWindowID(win1.window);
    win1.pen = (SDL_Rect){0, 0, wightpen, wightpen};
    win1.mouseX = 0;
    win1.mouseY = 0;

    // Windows 2 for colors
    win2.window = SDL_CreateWindow(
        "Couleur",
        SDL_WINDOWPOS_CENTERED + 50, SDL_WINDOWPOS_CENTERED + 50,
        400, 300, SDL_WINDOW_SHOWN
    );
    win2.renderer = SDL_CreateRenderer(win2.window, -1, SDL_RENDERER_ACCELERATED);
    win2.windowID = SDL_GetWindowID(win2.window);
    win2.pen = (SDL_Rect){0, 0, wightpen, wightpen};

    int running = 1;
    SDL_Event event;
    SDL_Rect rect = {0, 0, wightpen, wightpen};

    /* Rendu initial win1*/
    SDL_SetRenderDrawColor(win1.renderer, 255, 255, 255, 255);
    SDL_RenderClear(win1.renderer);
    SDL_SetRenderDrawColor(win1.renderer, 0, 0, 0, 255);
    SDL_RenderDrawPoint(win1.renderer, win1.mouseX, win1.mouseY);

    /* Rendu initial win2*/
    SDL_SetRenderDrawColor(win2.renderer, 255, 0, 0, 255); // Rouge
    SDL_RenderFillRect(win2.renderer, &(SDL_Rect){0, 0, 200, 150});
    SDL_SetRenderDrawColor(win2.renderer, 0, 255, 0, 255); // Vert
    SDL_RenderFillRect(win2.renderer, &(SDL_Rect){200, 0, 200, 150});
    SDL_SetRenderDrawColor(win2.renderer, 0, 0, 255, 255); // Bleu
    SDL_RenderFillRect(win2.renderer, &(SDL_Rect){0, 150, 200, 150});
    SDL_SetRenderDrawColor(win2.renderer, 0, 0, 0, 255); // Noir
    SDL_RenderFillRect(win2.renderer, &(SDL_Rect){200, 150, 200, 150});
    SDL_Color textColor = {0, 0, 0, 255}; // black color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Click on the color you want", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(win2.renderer, textSurface);
    SDL_Rect textRect = {50, 50, textSurface->w, textSurface->h}; // rectangle where the text is drawn 
    SDL_RenderCopy(win2.renderer, textTexture, NULL, &textRect);
    SDL_RenderPresent(win2.renderer);

    PaintWindow* activeWindow = NULL;

    while (running) {
        while (SDL_PollEvent(&event)) {
            /* Event Type Window*/
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    if (event.window.windowID == win1.windowID) {
                        activeWindow = &win1;
                        SDL_RaiseWindow(activeWindow->window);
                        SDL_SetWindowInputFocus(activeWindow->window);
                    } else if (event.window.windowID == win2.windowID) {
                        activeWindow = &win2;
                        SDL_RaiseWindow(activeWindow->window);
                        SDL_SetWindowInputFocus(activeWindow->window);
                    }
                }

                /* Quit */
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    running = 0;
                }
            }
            /* Event Window 1 */
            if (event.motion.windowID == win1.windowID) {
                if (event.type == SDL_MOUSEMOTION) {
                    rect.x = event.motion.x - rect.w / 2;
                    rect.y = event.motion.y - rect.h / 2;
                    if (mousePressed) {
                        SDL_RenderFillRect(win1.renderer, &rect);
                    }
                }
            }
            /* Event Window 2 */
            if (event.motion.windowID == win2.windowID) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    if (event.button.button == SDL_BUTTON_LEFT) {
                       if (event.button.x < 200 && event.button.y < 150) {
                           SDL_SetRenderDrawColor(win1.renderer, 255, 0, 0, 255); // Rouge
                       } else if (event.button.x >= 200 && event.button.y < 150) {
                           SDL_SetRenderDrawColor(win1.renderer, 0, 255, 0, 255); // Vert
                       } else if (event.button.x < 200 && event.button.y >= 150) {
                           SDL_SetRenderDrawColor(win1.renderer, 0, 0, 255, 255); // Bleu
                       } else {
                           SDL_SetRenderDrawColor(win1.renderer, 0, 0, 0, 255); // Noir
                       }
                    }
                }
            }
            /* Event Public */
            if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    wightpen += 1;
                } else if (event.wheel.y < 0 && wightpen > 1) {
                    wightpen -= 1;
                }
                rect.w = wightpen;
                rect.h = wightpen;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = true;
                    if (event.motion.windowID == win1.windowID) {
                        SDL_RenderFillRect(win1.renderer, &rect);
                        }
                }
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = false;
                }
            }
        }
        SDL_RenderPresent(win1.renderer);
    }

    SDL_DestroyRenderer(win1.renderer);
    SDL_DestroyWindow(win1.window);
    SDL_Quit();
    SDL_DestroyRenderer(win2.renderer);
    SDL_DestroyWindow(win2.window);
    SDL_Quit();
    return 0;
}