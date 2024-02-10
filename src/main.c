/*
#ifdef __unix__
    #include <SDL2/SDL.h>
#elif defined _WIN32
    #include <SDL.h>
#endif
*/
#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>

const int WIDTH = 800, HEIGHT = 800;

int main(void) {
    int quit = 1;
    SDL_Event e;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("SDL Template", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    while(quit) {
        SDL_PollEvent(&e);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        switch (e.type) {
            case SDL_QUIT: quit=0; break;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    return 0;
}
