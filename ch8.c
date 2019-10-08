#include "ch8.h"
#include <stdlib.h>

// Initializes fileds of ch8_State struct.
int ch8_init(ch8_State* state){
    for (int i=0; i<V_REGS_COUNT; i++){
        state->V[i] = 0;
    }
    state->I = 0;
    state->PC = 0x200;
    state->SP = 0;
    state->DT = 0;
    state->ST = 0;
    for (int i=0; i<RAM_SIZE; i++){
        state->ram[i] = 0;
    }
    for (int i=0; i<STACK_SIZE; i++){
        state->stack[i] = 0;
    }
    for (int i=0; i<VRAM_SIZE; i++){
        state->vram[i] = false;
    }
    for (int i=0; i<KEYS_COUNT; i++){
        state->keys[i] = false;
    }
    static uint8_t digit_sprites[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  //6
        0xF0, 0x10, 0x20, 0x40, 0x40,  //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  //9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  //A
        0xF0, 0x90, 0xF0, 0x90, 0x90,  //B
        0xF0, 0x90, 0xF0, 0x90, 0x90,  //C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };
    for (int i=0; i<80; i++){
        state->ram[i] = digit_sprites[i];
    }
    state->key_state = KEY_IGNORE;
    state->key_reg_idx = 0;
    return 0;
}

// Loads rom into memory
int ch8_load_rom(ch8_State* state, uint8_t *rom, uint16_t len){
    if(len > RAM_SIZE - 0x200){
        return 1;
    }

    for(uint16_t i=0; i<len; ++i){
        state->ram[0x200 + i] = rom[i];
    }
    return 0;
}

// Fetechs and executes next instruction pointed by PC.
int ch8_exec_next(ch8_State* state){
    if(state->key_state == KEY_WAIT){
        return 0;
    }
    if(state->key_state < KEYS_COUNT){
        state->V[state->key_reg_idx] = state->key_state;
        state->key_state = KEY_IGNORE;
        return 0;
    }
    uint8_t b1 = state->ram[state->PC];
    uint8_t b2 = state->ram[state->PC + 1];
    uint8_t n1 = b1 >> 4;
    uint8_t n2 = b1 & 0xF;
    uint8_t n3 = b2 >> 4;
    uint8_t n4 = b2 & 0xF;
    uint8_t kk = b2;
    uint16_t nnn = (((uint16_t)n3) << 8) + b1;
    uint8_t x = n2; uint8_t y = n3;
    state->PC += 2;

    switch(n1){
    case 0x0:
      if(n2 == 0x0 && n3 == 0xE && n4 == 0x0){ //CLS
        for (int i=0; i<VRAM_SIZE; ++i){
          state->vram[i] = false;
        }
      } else if(n2 == 0x0 && n3 == 0xE && n4 == 0xE){ //RET
        if(state->SP == 0){
          return 1;
        }
        state->SP -= 1;
        state->PC = state->stack[state->SP];
      } else{ //SYS
      }
      break;
    case 0x1: // JP nnn
      state->PC = nnn;
      break;
    case 0x2: // CALL nnn
      if (state->SP == STACK_SIZE - 1) {
        return 1;
      }
      state->stack[state->SP] = state->PC;
      state->SP += 1;
      state->PC = nnn;
      break;
    case 0x3: // SE Vx, kk
      if(state->V[x] == kk){
        state->PC += 2;
      }
      break;
    case 0x4: // SNE Vx, kk
      if(state->V[x] != kk){
        state->PC += 2;
      }
      break;
    case 0x5: // SE Vx, Vy
      if(state->V[x] == state->V[y]){
        state->PC += 2;
      }
      break;
    case 0x6: // LD Vx, kk
      state->V[x] = kk;
      break;
    case 0x7: // ADD Vx, kk
      state->V[x] += kk;
      break;
    case 0x8:
      switch(n4) {
      case 0x0: // LD Vx, Vy
        state->V[x] = state->V[y];
        break;
      case 0x1: // OR Vx, Vy
        state->V[x] |= state->V[y];
        break;
      case 0x2: // AND Vx, Vy
        state->V[x] &= state->V[y];
        break;
      case 0x3: // XOR Vx, Vy
        state->V[x] ^= state->V[y];
        break;
      case 0x4: { // ADD Vx, Vy
        uint16_t u16 = ((uint16_t) state->V[x]) + ((uint16_t) state->V[x]);
        state->V[0xF] = (u16 > 0xFF) ? 1 : 0;
        state->V[x] += state->V[y];
        break;
      }
      case 0x5: // SUB Vx, Vy
        state->V[0xF] = (state->V[x] > state->V[y]) ? 1 : 0;
        state->V[x] = state->V[x] - state->V[y];
        break;
      case 0x6: // SHR Vx
        state->V[0xF] = state->V[x] & 1;
        state->V[x] = state->V[x] >> 1;
        break;
      case 0x7: // SUBN Vx, Vy
        state->V[0xF] = (state->V[y] > state->V[x]) ? 1 : 0;
        state->V[x] = state->V[y] - state->V[x];
        break;
      case 0xE: // SHL Vx
        state->V[0xF] = (state->V[x] & 0x80) ? 1 : 0;
        state->V[x] = state->V[x] << 1;
        break;
      }
      break;
    case 0x9: // SNE Vx, Vy
      if(state->V[x] != state->V[y]){
        state->PC += 2;
      }
      break;
    case 0xA: // LD I, nnn
      state->I = nnn;
      break;
    case 0xB: // JP V0, nnn
      state->PC = nnn + state->V[0];
      break;
    case 0xC: // RND Vx, kk
      state->V[x] = (rand() % 256) & kk;
      break;
    case 0xD: {// DRW Vx, Vy, n
      uint8_t override_flag;
      uint16_t row, col, col_start;
      row = state->V[y] % SCREEN_HEIGHT;
      col = col_start = state->V[x] % SCREEN_WIDTH;
      override_flag = 0;

      // Y-axis loop
      for(int i=0; i<n4; i++) {
        uint8_t sprite = state->ram[state->I + i];

        // X-axis loop
        for(uint8_t mask=0x80; mask>0; mask=mask>>1) {
          bool new_pixel;
          uint16_t vram_idx;

          // Extract data about screen from sprite and calculate pos in VRAM.
          new_pixel = (sprite & mask) ? true : false;
          vram_idx = (row*SCREEN_WIDTH + col)%(SCREEN_WIDTH*SCREEN_HEIGHT);

          // If update will unset currently set pixel -> set override flag.
          if(state->vram[vram_idx] && new_pixel) {
            override_flag = 1;
          }
          state->vram[vram_idx] ^= new_pixel;

          // Move to next column
          col = (col+1) % SCREEN_WIDTH;
        }

        // Move to next row and reset column.
        row = (row+1) % SCREEN_HEIGHT;
        col = col_start;
      }

      state->V[0xF] = override_flag;
      break;
    }
    case 0xE: {
      uint8_t key_idx = state->V[x];
      switch(kk) {
      case 0x9E: // SKP Vx
        if (key_idx < KEYS_COUNT && state->keys[key_idx]) {
          state->PC += 2;
        }
        break;
      case 0xA1: // SKPN Vx
        if (key_idx < KEYS_COUNT && !state->keys[key_idx]) {
          state->PC += 2;
        }
        break;
      }
      break;
    }
    case 0xF:
      switch(kk) {
      case 0x15: // LD DT, Vx
        state->DT = state->V[x];
        break;
      case 0x18: // LD ST, Vx
        state->ST = state->V[x];
        break;
      case 0x1E: // ADD I, Vx
        state->I += state->V[x];
        break;
      case 0x29: // LD F, Vx
        state->I = state->V[x]*5;
        break;
      case 0x33: // LD B, Vx
        state->ram[state->I+0] = state->V[x]/100;
        state->ram[state->I+1] = (state->V[x]/10)%10;
        state->ram[state->I+2] = state->V[x]%10;
        break;
      case 0x55: // LD [I], Vx
        for(int i=0; i <= x  && i < 16; i++) {
          state->ram[state->I + i] = state->V[i];
        }
        break;
      case 0x65: // LD Vx, [I]
        for(int i=0; i <= x && i < 16; i++) {
          state->V[i] = state->ram[state->I + i];
        }
        break;
      }
      break;
    }
    return 0;
}

// Frame refreshed at 60Hz rate, so this can be used for decrementing DT, ST.
int ch8_on_frame_render(ch8_State* state){
    if(state->DT > 0){
        state->DT--;
    }
    if(state->ST > 0){
        state->ST--;
    }

    return 0;
}

// Updates keyboard state
int ch8_set_key(ch8_State* state, uint8_t idx, bool pressed){
    if(idx >= KEYS_COUNT){
        return 1;
    }
    if(state->key_state == KEY_WAIT && pressed){
        state->key_state = idx;
    }
    state->keys[idx] = pressed;
    return 0;
}
