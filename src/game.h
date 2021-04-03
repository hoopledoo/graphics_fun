/* date = July 31st 2020 9:59 am */

#ifndef GAME_H
#define GAME_H

/*
GAME_INTERNAL:
0 - Build for release
1 - dev build

GAME_SLOW:
0 - No slow code allowed
1 - slow code allowed
*/

#if GAME_SLOW
// Cause an exception if assertion fails
#define Assert(Expression) \
if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL);

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))
/* TODO: swap, min, max .... macros?? */

#define NumberOfDefinedKeys 52

inline uint32_t
SafeTruncateUInt64(uint64_t Value)
{
    Assert(Value <= 0xFFFFFFFF);
    uint32_t Result = (uint32_t) Value;
    return Result;
}

/*
 NOTE: Services that the game provides to the platform layer
(This may expand in the future - sound on separate thread, etc.)
*/

#if GAME_INTERNAL
/* NOTE:
 These are NOT for doing anything in the shipping game - they are 
blocking and the write doesn't protect against lost data!
It's JUST FOR DEBUG
*/
struct debug_read_file_result
{
    void *Content;
    uint32_t ContentSize;
};
internal debug_read_file_result DEBUGPlatformReadEntireFile(char *Filename);
internal bool32 DEBUGPlatformWriteEntireFile(char *Filename, uint32_t MemorySize, void *Memory);
internal void DEBUGPlatformFreeFileMemory(void *Memory);
#endif

/* 
Needs to take 4 things:
- timing
- controller/keyboard input
- bitmap to use 
- sound buffer to use

NOTE: In the future rendering _specifically_ will become a 3-tiered abstraction
*/
struct game_offscreen_buffer
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct game_memory
{
    bool32 IsInitialized;
    uint64_t PermanentStorageSize;
    void *PermanentStorage; // NOTE: REQUIRED to be cleared to zero @ startup
    uint64_t TransientStorageSize;
    void *TransientStorage; // NOTE: REQUIRED to be cleared to zero @ startup
};

// Array of all keys that can be used:
// [ESC, TAB, CAPS, SHIFT, LCTRL, LALT, SPAC, RALT, RCTRL, ...]
enum VKEY 
{
    ESC = 0,
    TAB = 1,
    CAPS = 2,
    LSHIFT = 3,
    LCTRL = 4,
    LALT = 5, 
    SPACE = 6,
    RALT = 7,
    RCTRL = 8,
    LEFT = 9,
    DOWN = 10,
    RIGHT = 11,
    UP = 12,
    RSHIFT = 13,
    ENTER = 14,
    BACKSPACE = 15,
    ONE = 16,
    TWO = 17,
    THREE = 18,
    FOUR = 19,
    FIVE = 20,
    SIX = 21,
    SEVEN = 22,
    EIGHT = 23,
    NINE = 24,
    ZERO = 25,
    Q = 26,
    W = 27,
    E = 28,
    R = 29,
    T = 30,
    Y = 31,
    U = 32,
    I = 33,
    O = 34,
    P = 35,
    A = 36,
    S = 37,
    D = 38,
    F = 39,
    G = 40,
    H = 41,
    J = 42,
    K = 43,
    L = 44,
    Z = 45,
    X = 46,
    C = 47, 
    V = 48,
    B = 49,
    N = 50,
    M = 51
};

// Position in the game_keyboard_input keys array will indicate the keycode,
// so it's not necessary to include that here.
// We only care to track the changes in state (pressed, released, up, down, num_presses)
// pressed is implied by prev_state_up = true, curr_state_up=false
// released is implied by prev_state_up = false, curr_state_up = true
struct game_key_state
{
    bool32 prev_state_up;
    bool32 curr_state_up;
    uint32_t num_presses;
};

// For now, we'll simply pass an array of all keys we care to track
// along with information about which keys were pressed or released
// And each individual key input item will track information about those keys.
// For quick lookup, we'll also track released keys, and newly pressed keys
struct game_keyboard_input
{
    uint32_t num_keys;
    game_key_state keys[NumberOfDefinedKeys];
    bool32 keys_released[NumberOfDefinedKeys];
    bool32 keys_pressed[NumberOfDefinedKeys];
};

internal void GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer, real32 delta_time, game_keyboard_input *Input);

/*
NOTE: Services that the platform layer provides to the game
*/

#endif //GAME_H
