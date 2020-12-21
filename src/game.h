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
-sound buffer to use

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

internal void GameUpdateAndRender(game_memory *Memory, game_offscreen_buffer *Buffer);

/*
NOTE: Services that the platform layer provides to the game
*/

#endif //GAME_H
