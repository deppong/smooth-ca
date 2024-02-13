#include <SDL.h>

#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_keycode.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_timer.h>
#include <SDL_video.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// window and board constants
#define BOARD_W 200
#define BOARD_H 200
const float RES = 800. / BOARD_W;

// simulation values (could make them dynamic with something like dearimgui)
float dt = 0.07f;
float ra = 12.f;
const float alpha_n = 0.01f;
const float alpha_m = 0.14f;
const float birthone = 0.257f;
const float birthtwo = 0.336f;
const float deathone = 0.365f;
const float deathtwo = 0.55f;

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) {
    return (a<<24) + (b<<16) + (g<<8) + r;
}

void draw_rect(int x, int y, int w, int h, uint8_t color, uint32_t *framedata) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            framedata[x+j + (y+i)*800] = pack_color(color, color, color, 255);
        }
    }
}

// Transition function: https://arxiv.org/pdf/1111.1567.pdf
float sigma_one(float x, float a, float alpha) {
    return 1.0f / (1.0f + expf(-(x - a) * 4 / alpha));
}
float sigma_two(float x, float a, float b) {
    return sigma_one(x, a, alpha_n) * (1 - sigma_one(x, b, alpha_n));
}
float sigma_m(float x, float y, float m) {
    return x * (1 - sigma_one(m, 0.5f, alpha_m)) + y * sigma_one(m, 0.5f, alpha_m);
}
float s(float n, float m) {
    return sigma_two(n, sigma_m(birthone, deathone, m), sigma_m(birthtwo, deathtwo, m));
}

void compute_cells(float cells[BOARD_H][BOARD_W], float next_cells[BOARD_H][BOARD_W]){
    for (int i = 0; i < BOARD_H; i++) {
        for (int j = 0; j < BOARD_W; j++) {
            // Kernel Inner and outer
            float kisum = 0.0f;
            float kosum = 0.0f;
            float kiarea = 0.0f;
            float koarea = 0.0f;
            float ri = 4.f;

            #define emod(x, y) (x%y + y)%y

            for (int y = -(ra-1); y <= ra-1; y++) {
                for (int x = -(ra-1); x <= ra-1; x++) {
                    if (x*x + y*y <= ri*ri) {
                        kisum += cells[(y+i+BOARD_H) % BOARD_H][(x+j+BOARD_W) % BOARD_W];
                        kiarea++;
                    } else if (x*x + y*y <= ra*ra) {
                        kosum += cells[(y+i+BOARD_H) % BOARD_H][(x+j+BOARD_W) % BOARD_W];
                        koarea++;
                    }
                }
            }
            kisum /= kiarea;
            kosum /= koarea;
            next_cells[i][j] = 2*s(kosum, kisum)-1;
        }
    }

    #define clamp(val, l, h) val < l ? l : (val > h ? h : val)

    // smooth time stepping
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            cells[y][x] += dt* next_cells[y][x];
            cells[y][x] = clamp(cells[y][x], 0, 1);
            // cells[y][x] = (cells[y][x] < 0.) ? 0. : (cells[y][x] > 1.) ? 1. : cells[y][x];
        }
    }
}

void compute_frame(uint32_t *framedata, float cells[BOARD_H][BOARD_W]) {
    for(int i = 0; i < BOARD_H; i++) {
        for(int j = 0; j < BOARD_W; j++) {
            draw_rect(j*RES, i*RES, RES, RES, cells[i][j]*255, framedata);
        }
    }
}

int main(void) {
    int quit = 1;
    SDL_Event e;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Smooth Cellular Automata", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 800, 800);
    if (!framebuffer) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    uint32_t *framedata = malloc(sizeof(uint32_t) *800*800);
    if (!framedata) {
        fprintf(stderr, "Failed to allocate framebuffer\n");
        return EXIT_FAILURE;
    }

    srand(time(0));

    float cells[BOARD_H][BOARD_W] = {0};
    float next_cells[BOARD_H][BOARD_W] = {0};
    for (int y = 0; y < BOARD_H; y++) {
        for (int x = 0; x < BOARD_W; x++) {
            cells[y][x] = (float)rand() / RAND_MAX;
            next_cells[y][x] = (float)rand() / RAND_MAX;
        }
    }

    float frame_dt = 0.f;
    uint32_t last_time = SDL_GetTicks();

    while(quit) {
        SDL_PollEvent(&e);

        switch (e.type) {
            case SDL_QUIT: quit=0; break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = 0; break;
                    case SDLK_SPACE:
                        for (int y = 0; y < BOARD_H; y++) {
                            for (int x = 0; x < BOARD_W; x++) {
                                cells[y][x] = (float)rand() / RAND_MAX;
                                next_cells[y][x] = (float)rand() / RAND_MAX;
                            }
                        }
                        break;
                } break;
        }

        frame_dt = ((float)SDL_GetTicks() - (float)last_time) / 1000.0f;

        if (floor(frame_dt / (1.0f / 50.f)) > 0.f) {
            compute_cells(cells, next_cells);
            compute_frame(framedata, cells);
            // memcpy(cells, next_cells, sizeof(float)*BOARD_W*BOARD_H);
            last_time = SDL_GetTicks();
        }

        // Render loop: move pointer of framedata to the texture, then do 1 draw call in SDL_RenderCopy();
        SDL_RenderClear(renderer);

        SDL_UpdateTexture(framebuffer, NULL, framedata, 800*4);

        SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(framebuffer);

    free(framedata);

    return EXIT_SUCCESS;
}
