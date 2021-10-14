/* date = August 14th 2020 0:19 pm */

#ifndef WIN32_GAME_H
#define WIN32_GAME_H

// TODO: We may want to handle this differently later
#define BYTESPERPIXEL 4

// This is temporary, TODO: we want to better handle window sizing later
#define WIDTH 1280
#define HEIGHT 720

#include "game.h"

// TODO: Create a more robust system for handling these paths
char* OrigGameDLLPath = "C:\\dev\\graphics_fun\\build\\game.dll";
char* TempGameDLLPath = "C:\\dev\\graphics_fun\\build\\game_temp.dll";

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	void* Memory;
	int Width;
	int Height;
	int Pitch;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

struct win32_sound_output
{
	// NOTE: Variables used for sound test
	int SamplesPerSecond;
	uint32_t RunningSampleIndex;
    int BytesPerSample;
    DWORD SecondaryBufferSize;
    real32 tSine;
    int LatencySampleCount;
};

struct win32_debug_sound_cursors
{
	DWORD PlayCursor;
	DWORD WriteCursor;
};

// NOTE: Support for DirectSoundCreate (rather than linking directly against dsound.dll)
#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

#if 0 /// TODO: Properly introduce XINPUT Handling
// ------------------------- NOTE: These are necessary for xinput -----------------------------------
// NOTE: Support for XInputGetState (rather than linking against XInput.lib)
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState) 
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{  										// Initialize the function we intend to load dynamically
	return(ERROR_DEVICE_NOT_CONNECTED);	// with an empty stub to avoid breaking if we call & it's
}										// not found externally.
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_


// NOTE: Support for XInputSetState (rather than linking against XInput.lib)
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration) 
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{  										// Initialize the function we intend to load dynamically
	return(ERROR_DEVICE_NOT_CONNECTED);	// with an empty stub to avoid breaking if we call & it's
}										// not found externally.
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_
#endif

#endif //WIN32_GAME_H
