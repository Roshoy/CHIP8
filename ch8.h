#include<stdint.h>
#include<stdbool.h>

#ifndef CH8
#define CH8

#define V_REGS_COUNT 1 << 4;
#define KEYS_COUNT   1 << 4;
#define STACK_SIZE   1 << 4;
#define RAM_SIZE     1 << 12;
#define VRAM_SIZE    1 << 11;
#define KEY_WAIT     0xFF;
#define KEY_IGNORE   0x80;

typedef struct ch8_State {
    // Registers
    uint8_t V[V_REGS_COUNT];
    uint16_t I;
    uint16_t PC;
    uint8_t SP;
    uint8_t DT;
    uint8_t ST;
    bool VF;
    // Memory
    uint8_t ram[RAM_SIZE];
    uint16_t stack[STACK_SIZE];
    // Vram
    bool vram[VRAM_SIZE];
    // Keyboard state
    bool keys[KEYS_COUNT];
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