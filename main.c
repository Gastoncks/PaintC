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

bool isMouseOver(int mx, int my, SDL_Rect r) { /* Check if mouse is over a rectangle */
    return (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h);
}

void renderButton(SDL_Renderer* renderer, Button button, int Mouse_x, int Mouse_y, SDL_Event event) {
    if (isMouseOver(Mouse_x, Mouse_y, button.rect)) {
        if (event.button.button == SDL_BUTTON_LEFT && event.type == SDL_MOUSEBUTTONDOWN) { 
            /*Active Button*/
            SDL_SetRenderDrawColor(renderer, button.activeColor.r, button.activeColor.g, button.activeColor.b, button.activeColor.a);
            SDL_RenderFillRect(renderer, &button.rect);
        } else {
            /*Hover Button*/
            SDL_SetRenderDrawColor(renderer, button.hoverColor.r, button.hoverColor.g, button.hoverColor.b, button.hoverColor.a);
            SDL_RenderFillRect(renderer, &button.rect);
        }
    } else {
        /*Base Button*/
        SDL_SetRenderDrawColor(renderer, button.baseColor.r, button.baseColor.g, button.baseColor.b, button.baseColor.a);
        SDL_RenderFillRect(renderer, &button.rect);
    }
}

void renderGauge(SDL_Renderer* renderer, Gauge gauge) { /*Render Gauge*/
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
    fillRect.x = gauge.x;
    fillRect.w = gauge.w;
    fillRect.h = fillHeight;
    fillRect.y = (gauge.y + gauge.h) - fillHeight;
    /* Draw the fill rectangle*/
    SDL_SetRenderDrawColor(renderer, gauge.color.r, gauge.color.g, gauge.color.b, gauge.color.a); 
    SDL_RenderFillRect(renderer, &fillRect);

    /* Border */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // noir
    SDL_RenderDrawRect(renderer, &bgRect);
}

/* Update gauge value if isMouseOver and Up or Down press */
void UpdateIfSelectedGauge(Gauge* gauge, int mouseX,  int mouseY, SDL_Event event, SDL_Renderer* renderer) { 
    if (isMouseOver(mouseX, mouseY, (SDL_Rect){gauge->x, gauge->y, gauge->w, gauge->h})) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                gauge->value += 5;
                if (gauge->value > 255) gauge->value = 255;
                printf("Valeur: %d\n", gauge->value);
                renderGauge(renderer, *gauge);
                break;
            case SDLK_DOWN:
                gauge->value -= 5;
                if (gauge->value < 0) gauge->value = 0;
                printf("Valeur: %d\n", gauge->value);
                renderGauge(renderer, *gauge);
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
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
    SDL_SetRenderDrawBlendMode(win1.renderer, SDL_BLENDMODE_BLEND);
    win1.windowID = SDL_GetWindowID(win1.window);
    win1.pen = (SDL_Rect){0, 0, wightpen, wightpen};

    // Windows 2 for color selection
    
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
    SDL_SetRenderDrawColor(win1.renderer, 50, 50, 50, 255);

    /* Rendu initial win2*/
    int mouseX_2, mouseY_2;

    SDL_Color textColor = {0, 0, 0, 255}; // black color for text

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Clique on the button after selecting the color", textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(win2.renderer, textSurface);
    SDL_Rect textRect = {70, 15, textSurface->w, textSurface->h}; // rectangle where the text is drawn 

    Button setcolorbutton = {(SDL_Rect){10, 10, 60, 30},(SDL_Color){100, 100, 100, 255},
        (SDL_Color){150, 150, 150, 255},(SDL_Color){0, 200, 0, 255}, false};

    SDL_SetRenderDrawColor(win2.renderer, 255, 255, 255, 255);
    SDL_RenderClear(win2.renderer);

    PaintWindow* activeWindow = NULL;

    Gauge RedGauge = {50, 50, 10, 100, {255, 0, 0, 255}, 50};
    Gauge GreenGauge = {100, 50, 10, 100, {0, 255, 0, 255}, 50};
    Gauge BlueGauge = {150, 50, 10, 100, {0, 0, 255, 255}, 50};
    Gauge AlphaGauge = {200, 50, 10, 100, {255, 255, 255, 255}, 255};

    renderButton(win2.renderer, setcolorbutton, mouseX_2, mouseY_2, event); //button
    SDL_RenderCopy(win2.renderer, textTexture, NULL, &textRect); // text
    renderGauge(win2.renderer, RedGauge); //gauges
    renderGauge(win2.renderer, GreenGauge);
    renderGauge(win2.renderer, BlueGauge);
    renderGauge(win2.renderer, AlphaGauge);

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
                renderButton(win2.renderer, setcolorbutton, mouseX_2, mouseY_2, event);
                if (event.type == SDL_KEYDOWN) {
                // Up and Down to change gauge value
                    UpdateIfSelectedGauge(&RedGauge, mouseX_2, mouseY_2, event, win2.renderer);
                    UpdateIfSelectedGauge(&GreenGauge, mouseX_2, mouseY_2, event, win2.renderer);
                    UpdateIfSelectedGauge(&BlueGauge, mouseX_2, mouseY_2, event, win2.renderer);
                    UpdateIfSelectedGauge(&AlphaGauge, mouseX_2, mouseY_2, event, win2.renderer);
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
                    SDL_SetRenderDrawColor(win1.renderer, RedGauge.value, GreenGauge.value, BlueGauge.value, AlphaGauge.value);
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