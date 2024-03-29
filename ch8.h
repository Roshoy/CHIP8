#include<stdint.h>
#include<stdbool.h>

#ifndef CH8_H
#define CH8_H

#define CH8_V_REG_COUNT (1 << 4)
#define CH8_KEYS_COUNT   (1 << 4)
#define CH8_STACK_SIZE   (1 << 4)
#define CH8_RAM_SIZE     (1 << 12)
#define CH8_VRAM_SIZE    (1 << 11)
#define CH8_KEY_WAIT     0xFF
#define CH8_KEY_IGNORE   0x80
#define CH8_VRAM_WIDTH 64
#define CH8_VRAM_HEIGHT 32

typedef struct ch8_State {
    // Registers
    uint8_t V[CH8_V_REG_COUNT];
    uint16_t I;
    uint16_t PC;
    uint8_t SP;
    uint8_t DT;
    uint8_t ST;
    // Memory
    uint8_t ram[CH8_RAM_SIZE];
    uint16_t stack[CH8_STACK_SIZE];
    // Vram
    bool vram[CH8_VRAM_SIZE];
    // Keyboard state
    bool keys[CH8_KEYS_COUNT];
    // Key to wait for
    uint8_t key_state;
    uint8_t key_reg_idx;
} ch8_State;

// Initializes fileds of ch8_State struct.
int ch8_init(ch8_State*);

// Loads rom into memory
int ch8_load_rom(ch8_State*, uint8_t*, uint16_t);

// Fetechs and executes next instruction pointed by PC.
int ch8_exec_next(ch8_State*);

// Frame refreshed at 60Hz rate, so this can be used for decrementing DT, ST.
int ch8_on_frame_render(ch8_State*);

// Updates keyboard state
int ch8_set_key(ch8_State*, uint8_t, bool);

#endif
