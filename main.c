#include "include/SDL2/SDL.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erreur d'initialisation SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Fenetre
    SDL_Window* window = SDL_CreateWindow(
        "GastonPaint",        // Titre
        SDL_WINDOWPOS_CENTERED,     // Position X
        SDL_WINDOWPOS_CENTERED,     // Position Y
        800,                        // Largeur
        600,                        // Hauteur
        SDL_WINDOW_SHOWN            // Flags
    );
    if (!window) {
        printf("Erreur de création de fenêtre: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    int mouseX = 0;
    int mouseY = 0;
    unsigned int wightpen = 10;
    bool mousePressed = false;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect rect = {0, 0, wightpen, wightpen};
    SDL_RenderFillRect(renderer, &rect);
    SDL_RenderDrawPoint(renderer, mouseX, mouseY);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mousePressed = true;
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
                    mousePressed = false;
                }
            }
            if (event.type == SDL_MOUSEMOTION) {
                rect.x = event.motion.x - rect.w / 2;
                rect.y = event.motion.y - rect.h / 2;
                if (mousePressed) {
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}