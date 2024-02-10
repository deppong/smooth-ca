#include <SDL.h>

#include <SDL_error.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_video.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

const int WIDTH = 400, HEIGHT = 400;

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
    return (a<<24) + (b<<16) + (g<<8) + r;
}

void compute_cells(float *cells, float *next_cells){
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {

            // neighbors
            float kisum = 0.0f;
            float kosum = 0.0f;
            for (int y = -11; y < 12; y++) {
                for (int x = -11; x < 12; x++) {
                    if (x >= -1 && x < 2 && y >= -1 && y < 2) {
                        kisum += cells[(j+x+HEIGHT)%HEIGHT + (i+y+WIDTH)%WIDTH * WIDTH];
                    }
                    kosum += cells[(j+x+HEIGHT)%HEIGHT + (i+y+WIDTH)%WIDTH * WIDTH];
                }
            }
            kisum /= 9.f;
            kosum /= 529.f;
            if ((kisum >= 0.5f && 0.26f <= kosum && kosum <= 0.46f) ||
                (kisum < 0.5f && 0.27f <= kosum && kosum <= 0.36f)) {
                next_cells[j + i*WIDTH] = 1.f;
            } else {
                next_cells[j + i*WIDTH] = 0.0f;
            }
            // if (kosum >= 0.5f) {
            //     next_cells[j + i*WIDTH] = 1.0f;
            // } else {
            //     next_cells[j + i*WIDTH] = 0.0f;
            // }

        }
    }
}

void compute_frame(uint32_t *framedata, float *cells) {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            framedata[j + i * WIDTH] = pack_color(cells[j + i*WIDTH]*255, cells[j + i*WIDTH]*255, cells[j + i*WIDTH]*255, cells[j + i*WIDTH]*255);
        }
    }
}

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

    srand(time(0));

    float *cells = malloc(sizeof(float) * WIDTH*HEIGHT);
    float *next_cells = malloc(sizeof(float) * WIDTH*HEIGHT);
    for(int i = 0; i < WIDTH*HEIGHT; i++){
        cells[i] = (float)rand() / RAND_MAX;
        next_cells[i] = (float)rand() / RAND_MAX;
    }

    while(quit) {
        SDL_PollEvent(&e);

        switch (e.type) {
            case SDL_QUIT: quit=0; break;
        }

        // Compute
        compute_cells(cells, next_cells);
        compute_frame(framedata, next_cells);
        cells = next_cells;

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
    free(cells);
    free(next_cells);

    return EXIT_SUCCESS;
}
