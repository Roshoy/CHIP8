// Adrian Maciej super gosciu
#include "ch8.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

// Define screen dimensions
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 320

#define ROM_BUFFSZ (1<<12)

void rect(SDL_Renderer *render, int x, int y, int w, int h) {
  SDL_Rect rect;
  rect.w = w;
  rect.h = h;
  rect.x = x;
  rect.y = y;
  SDL_RenderFillRect(render, &rect);
}

int main(int argc, char **argv){
  (void) argc;

  int rom_size;
  char rom_buff[ROM_BUFFSZ];

  printf("ROM should be passed via STDIN.\n"
         "Example: 'cat [rom_path$(CFLAGS)] | %s'\n",
         argv[0]);

  // Load ROM
  if ((rom_size = fread(rom_buff, 1, ROM_BUFFSZ, stdin)) < 0) {
    perror("Unable to read ROM from STDIN.\n");
    exit(1);
  }
  printf("--------------------\n"
         "Sucessfully read ROM.\n"
         "ROM SIZE: %d bytes\n", rom_size);

  // Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
      printf("SDL could not be initialized!\n"
             "SDL_Error: %s\n",
             SDL_GetError());
      return 1;
    }

    // Create window
    SDL_Window *window;
    if(!(window = SDL_CreateWindow("Chip 8 Prototype",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_SHOWN))) {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
        exit(1);
    }

    // Create renderer
    SDL_Renderer *renderer;
    if(!(renderer = SDL_CreateRenderer(window,
                                       -1,
                                       SDL_RENDERER_ACCELERATED))) {
      printf("Renderer could not be created!\n"
             "SDL_Error: %s\n",
             SDL_GetError());
      exit(1);
    }

    SDL_Event event;

    bool emulating = false;
    int x_ratio = WINDOW_WIDTH/CH8_VRAM_WIDTH;
    int y_ratio = WINDOW_HEIGHT/CH8_VRAM_HEIGHT;

    ch8_State chip8;
    ch8_init(&chip8);

    if(ch8_load_rom(&chip8, (uint8_t*) rom_buff, (uint16_t) rom_size) != 0) {
      printf("ROM load failed.\n");
      exit(1);
    }

    // Event loop
    while(!emulating) {
      for(int i=0; i<5; i++){
        int err;
        if((err = ch8_exec_next(&chip8)) != 0){
          printf("ch8_exec_next() failed with code %d\n", err);
          printf("PC: %x", chip8.PC);
          exit(1);
        }
      }

      // Handle event queue
      while(SDL_PollEvent(&event) != 0) {
        switch(event.type) {
        case SDL_QUIT:
          emulating = true;
          break;
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
          case SDLK_1: { ch8_set_key(&chip8, 0x1, true); break; }
          case SDLK_2: { ch8_set_key(&chip8, 0x2, true); break; }
          case SDLK_3: { ch8_set_key(&chip8, 0x3, true); break; }
          case SDLK_4: { ch8_set_key(&chip8, 0xC, true); break; }
          case SDLK_q: { ch8_set_key(&chip8, 0x4, true); break; }
          case SDLK_w: { ch8_set_key(&chip8, 0x5, true); break; }
          case SDLK_e: { ch8_set_key(&chip8, 0x6, true); break; }
          case SDLK_r: { ch8_set_key(&chip8, 0xD, true); break; }
          case SDLK_a: { ch8_set_key(&chip8, 0x7, true); break; }
          case SDLK_s: { ch8_set_key(&chip8, 0x8, true); break; }
          case SDLK_d: { ch8_set_key(&chip8, 0x9, true); break; }
          case SDLK_f: { ch8_set_key(&chip8, 0xE, true); break; }
          case SDLK_z: { ch8_set_key(&chip8, 0xA, true); break; }
          case SDLK_x: { ch8_set_key(&chip8, 0x0, true); break; }
          case SDLK_c: { ch8_set_key(&chip8, 0xB, true); break; }
          case SDLK_v: { ch8_set_key(&chip8, 0xF, true); break; }
          break;
          }
          break;
        case SDL_KEYUP:
          switch(event.key.keysym.sym) {
          case SDLK_1: { ch8_set_key(&chip8, 0x1, false); break; }
          case SDLK_2: { ch8_set_key(&chip8, 0x2, false); break; }
          case SDLK_3: { ch8_set_key(&chip8, 0x3, false); break; }
          case SDLK_4: { ch8_set_key(&chip8, 0xC, false); break; }
          case SDLK_q: { ch8_set_key(&chip8, 0x4, false); break; }
          case SDLK_w: { ch8_set_key(&chip8, 0x5, false); break; }
          case SDLK_e: { ch8_set_key(&chip8, 0x6, false); break; }
          case SDLK_r: { ch8_set_key(&chip8, 0xD, false); break; }
          case SDLK_a: { ch8_set_key(&chip8, 0x7, false); break; }
          case SDLK_s: { ch8_set_key(&chip8, 0x8, false); break; }
          case SDLK_d: { ch8_set_key(&chip8, 0x9, false); break; }
          case SDLK_f: { ch8_set_key(&chip8, 0xE, false); break; }
          case SDLK_z: { ch8_set_key(&chip8, 0xA, false); break; }
          case SDLK_x: { ch8_set_key(&chip8, 0x0, false); break; }
          case SDLK_c: { ch8_set_key(&chip8, 0xB, false); break; }
          case SDLK_v: { ch8_set_key(&chip8, 0xF, false); break; }
          }
          break;
        }
      }

      // Clear background
      SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(renderer);

      // Set draw color to RED.
      SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

      // Render VRAM contents
      for(int y=0; y < CH8_VRAM_HEIGHT; y++){
        for(int x=0; x < CH8_VRAM_WIDTH; x++) {
          if (chip8.vram[y*CH8_VRAM_WIDTH + x]){
            rect(renderer, x*x_ratio, y*y_ratio, x_ratio, y_ratio);
          }
        }
      }
      ch8_on_frame_render(&chip8);

      // Flush changes
      SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
