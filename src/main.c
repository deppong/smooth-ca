#include <SDL.h>

#include <SDL_error.h>
#include <SDL_pixels.h>
#include <SDL_render.h>

#include <SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
    return (a<<24) + (b<<16) + (g<<8) + r;
}

const int WIDTH = 800, HEIGHT = 800;

int main(void) {
    int quit = 1;
    SDL_Event e;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Smooth Cellular Automata", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (!framebuffer) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    uint32_t *framedata = malloc(sizeof(uint32_t) * WIDTH*HEIGHT);
    if (!framedata) {
        fprintf(stderr, "Failed to allocate framebuffer\n");
        return EXIT_FAILURE;
    }

    // load pixels into the framedata
    for (int i = 0; i < WIDTH*HEIGHT; i++) {
        framedata[i] = pack_color(60, 70, 70, 255);
    }


    while(quit) {
        SDL_PollEvent(&e);

        switch (e.type) {
            case SDL_QUIT: quit=0; break;
        }

        // Render loop: move pointer of framedata to the texture, then do 1 draw call in SDL_RenderCopy();
        SDL_RenderClear(renderer);
        SDL_UpdateTexture(framebuffer, NULL, framedata, WIDTH*4);
        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(framebuffer);

    free(framedata);

    return EXIT_SUCCESS;
}
