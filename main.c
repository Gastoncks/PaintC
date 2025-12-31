#include "include/SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MY_FONT "C:\\Windows\\Fonts\\arial.ttf"
typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint32 windowID;
    SDL_Rect pen;
} PaintWindow;

typedef struct {
    SDL_Rect rect;       
    SDL_Color baseColor;
    SDL_Color hoverColor;
    SDL_Color activeColor;
    bool isActive;
} Button;

typedef struct {
    int x, y, w, h;
    SDL_Color color; //gauge color
    int value;   //gauge value (0-255)
} Gauge;

bool isMouseOver(int mx, int my, SDL_Rect r) {
    return (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h);
}

void UpdateIfSelectedGauge(Gauge* gauge, int mouseX,  int mouseY) {
    if (isMouseOver(mouseX, mouseY, (SDL_Rect){gauge->x, gauge->y, gauge->w, gauge->h})) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                gauge->value += 5;
                if (gauge->value > 255) gauge->value = 255;
                printf("Valeur: %d\n", gauge->value);
                renderGauge(win2.renderer, *gauge);
                break;
            case SDLK_DOWN:
                gauge->value -= 5;
                if (gauge->value < 0) gauge->value = 0;
                printf("Valeur: %d\n", gauge->value);
                renderGauge(win2.renderer, *gauge);
                break;
        }
    }
}

void renderGauge(SDL_Renderer* renderer, Gauge gauge) {
    /* Lock value between 0 and 255 */
    if (gauge.value < 0) gauge.value = 0;
    if (gauge.value > 255) gauge.value = 255;

    /* Draw the background */
    SDL_Rect bgRect = {gauge.x, gauge.y, gauge.w, gauge.h};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // Gris foncé
    SDL_RenderFillRect(renderer, &bgRect);

    /* Calculate the fill rectangle height */
    float percent = (float)gauge.value / 255.0f;
    int fillHeight = (int)(gauge.h * percent);

    /* Set the longeur of the fill rectangle */
    SDL_Rect fillRect;
    fillRect.x = gauge->x;
    fillRect.w = gauge->w;
    fillRect.h = fillHeight;
    fillRect.y = (gauge->y + gauge->h) - fillHeight;
    /* Draw the fill rectangle*/
    SDL_SetRenderDrawColor(renderer, gauge->color.r, gauge->color.g, gauge->color.b, gauge->color.a); 
    SDL_RenderFillRect(renderer, &fillRect);

    /* Border */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // noir
    SDL_RenderDrawRect(renderer, &bgRect);
}

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

    // Windows 2 for colors
    
    Button setcolorbutton;
    setcolorbutton.rect = (SDL_Rect){10, 10, 60, 30};
    setcolorbutton.baseColor = (SDL_Color){100, 100, 100, 255};
    setcolorbutton.hoverColor = (SDL_Color){150, 150, 150, 255};
    setcolorbutton.activeColor = (SDL_Color){0, 200, 0, 255};
    
    win2.window = SDL_CreateWindow(
        "Couleur",
        SDL_WINDOWPOS_CENTERED + 50, SDL_WINDOWPOS_CENTERED + 50,
        400, 300, SDL_WINDOW_SHOWN
    );
    win2.renderer = SDL_CreateRenderer(win2.window, -1, SDL_RENDERER_ACCELERATED);
    win2.windowID = SDL_GetWindowID(win2.window);
    win2.pen = (SDL_Rect){0, 0, wightpen, wightpen};

    bool running = 1;
    SDL_Event event;
    SDL_Rect rect = {0, 0, wightpen, wightpen};

    /* Rendu initial win1*/
    SDL_SetRenderDrawColor(win1.renderer, 255, 255, 255, 255);
    SDL_RenderClear(win1.renderer);
    SDL_SetRenderDrawColor(win1.renderer, 0, 0, 0, 255);

    /* Rendu initial win2*/
    SDL_Color textColor = {0, 0, 0, 255}; // black color
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Click on the color you want", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(win2.renderer, textSurface);
    SDL_Rect textRect = {50, 50, textSurface->w, textSurface->h}; // rectangle where the text is drawn 
    SDL_RenderCopy(win2.renderer, textTexture, NULL, &textRect);


    SDL_SetRenderDrawColor(win2.renderer, 255, 255, 255, 255);
    SDL_RenderClear(win2.renderer);
    SDL_SetRenderDrawColor(win2.renderer, setcolorbutton.baseColor.r, setcolorbutton.baseColor.g, setcolorbutton.baseColor.b, setcolorbutton.baseColor.a);
    SDL_RenderFillRect(win2.renderer, &setcolorbutton.rect);

    PaintWindow* activeWindow = NULL;

    Gauge RedGauge = {150, 100, 10, 100, {255, 0, 0, 255}, 50};
    renderGauge(win2.renderer, RedGauge);

    int mouseX_2, mouseY_2;

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
                    running = false;
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
                Uint32 mouseState = SDL_GetMouseState(&mouseX_2, &mouseY_2);
                /* Check if mouse is over the button */
                if (isMouseOver(mouseX_2, mouseY_2, setcolorbutton.rect)) {
                    if (event.button.button == SDL_BUTTON_LEFT && event.type == SDL_MOUSEBUTTONDOWN) {
                        SDL_SetRenderDrawColor(win2.renderer, setcolorbutton.activeColor.r,
                            setcolorbutton.activeColor.g, setcolorbutton.activeColor.b, setcolorbutton.activeColor.a);
                        setcolorbutton.isActive = true;
                        SDL_RenderFillRect(win2.renderer, &setcolorbutton.rect);
                    } else {
                        SDL_SetRenderDrawColor(win2.renderer, setcolorbutton.hoverColor.r,
                            setcolorbutton.hoverColor.g, setcolorbutton.hoverColor.b, setcolorbutton.hoverColor.a);
                        SDL_RenderFillRect(win2.renderer, &setcolorbutton.rect);
                    }
                } else {
                    SDL_SetRenderDrawColor(win2.renderer, setcolorbutton.baseColor.r,
                        setcolorbutton.baseColor.g, setcolorbutton.baseColor.b, setcolorbutton.baseColor.a);
                    SDL_RenderFillRect(win2.renderer, &setcolorbutton.rect);
                }
                if (event.type == SDL_KEYDOWN) {
                // Up and Down to change gauge value
                    UpdateIfSelectedGauge(&RedGauge, mouseX_2, mouseY_2);
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
                if (setcolorbutton.isActive == true) {
                    // Change pen color to green when button is active
                    SDL_SetRenderDrawColor(win1.renderer, 0, 255, 0, 255);
                    setcolorbutton.isActive = false;
                }
            }
        }
        SDL_RenderPresent(win1.renderer);
        SDL_RenderPresent(win2.renderer);
    }

    SDL_DestroyRenderer(win1.renderer);
    SDL_DestroyWindow(win1.window);
    SDL_Quit();
    SDL_DestroyRenderer(win2.renderer);
    SDL_DestroyWindow(win2.window);
    SDL_Quit();
    return 0;
}