#include "include/SDL2/SDL.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 1;
    }

    typedef struct {
        SDL_Window* window;
        SDL_Renderer* renderer;
        Uint32 windowID;
        bool mousePressed;
        SDL_Rect pen;
        int mouseX;
        int mouseY;
    } PaintWindow;

    PaintWindow win1;
    PaintWindow win2;

    unsigned int wightpen = 10;

    // Fenêtre 1
    win1.window = SDL_CreateWindow(
        "GastonPaint",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN
    );
    win1.renderer = SDL_CreateRenderer(win1.window, -1, SDL_RENDERER_ACCELERATED);
    win1.windowID = SDL_GetWindowID(win1.window);
    win1.mousePressed = false;
    win1.pen = (SDL_Rect){0, 0, wightpen, wightpen};
    win1.mouseX = 0;
    win1.mouseY = 0;

    // Fenêtre 2
    win2.window = SDL_CreateWindow(
        "Couleur",
        SDL_WINDOWPOS_CENTERED + 50, SDL_WINDOWPOS_CENTERED + 50,
        400, 300, SDL_WINDOW_SHOWN
    );
    win2.renderer = SDL_CreateRenderer(win2.window, -1, SDL_RENDERER_ACCELERATED);
    win2.windowID = SDL_GetWindowID(win2.window);
    win2.mousePressed = false;
    win2.pen = (SDL_Rect){0, 0, wightpen, wightpen};

    int running = 1;
    SDL_Event event;

    SDL_SetRenderDrawColor(win1.renderer, 255, 255, 255, 255);
    SDL_RenderClear(win1.renderer);
    SDL_SetRenderDrawColor(win1.renderer, 0, 0, 0, 255);
    SDL_Rect rect = {0, 0, wightpen, wightpen};
    SDL_RenderFillRect(win1.renderer, &win1.pen);
    SDL_RenderDrawPoint(win1.renderer, win1.mouseX, win1.mouseY);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    running = 0;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    win1.mousePressed = true;
                }
            }
            if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y > 0) {
                    wightpen += 1;
                } else if (event.wheel.y < 0 && wightpen > 1) {
                    wightpen -= 1;
                }
                rect.w = wightpen;
                rect.h = wightpen;
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    win1.mousePressed = false;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                rect.x = event.motion.x - rect.w / 2;
                rect.y = event.motion.y - rect.h / 2;
                if (win1.mousePressed) {
                    SDL_RenderFillRect(win1.renderer, &rect);
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