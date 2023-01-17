#include <windows.h>
#include <stdint.h>
#include <xinput.h>

//unsigned integers
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

//signed integers
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;

#define internal static
#define local_persist static
#define global_variable static

typedef struct win32_offscreen_buffer
{
    //NOTE(casey): Pixels are always 32-bits wide
    //Memory Order  0x BB GG RR xx
    //Little Endian 0x xx RR GG BB
    BITMAPINFO Info; //to describe our bitmap
    void *Memory; //a block memory for the purpose of drawing pixels into.
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
}win32_offscreen_buffer;

typedef struct win32_window_dimension
{
    int Width;
    int Height;
}win32_window_dimension;

global_variable b32 GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state* XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return (ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state* XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput()
{
    HMODULE XInputLibrary = LoadLibraryA("Xinput1_4.dll");
    if(!XInputLibrary)
    {
        XInputLibrary = LoadLibraryA("Xinput9_1_0.dll");
    }
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state*)GetProcAddress(XInputLibrary,"XInputGetState");
        if(!XInputGetState)
        {
            XInputGetState = XInputGetStateStub;
        }
        XInputSetState = (x_input_set_state*)GetProcAddress(XInputLibrary,"XInputSetState");
        if(!XInputSetState)
        {
            XInputSetState = XInputSetStateStub;
        }
    }
    else
    {
        XInputGetState = XInputGetStateStub;
        XInputSetState = XInputSetStateStub;
    }
}

internal win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    
    RECT ClientRect;
    GetClientRect(Window,&ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    
    return (Result);
}

internal void
RenderWeirdGradient(win32_offscreen_buffer *Buffer, int XOffset, int YOffset)
{
    u8 *Row = (u8 *)Buffer->Memory;
    for(int Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        u32* Pixel = (u32*)Row;
        for(int X = 0;
            X < Buffer->Width;
            ++X)
        {
            //pixel in memory: BB GG RR XX
            u8 Red = 0;
            u8 Green = (u8)( Y + YOffset);
            u8 Blue = (u8)(X + XOffset);
            *Pixel++= Red << 16 | Green << 8 | Blue;
        }
        Row += Buffer->Pitch;
    }
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    if(Buffer->Memory)
    {
        VirtualFree(Buffer->Memory,0,MEM_RELEASE);
    }
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    // NOTE(casey): when the biHeight field is negative, this is the clue
    // to Windows to treat this bitmap as top-down, not bottom-up, meaning
    // that the first bytes of the image are the color for the top left
    // pixel in the bitmap, not the bottom left!
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height; //negative value: top-down pitch
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    
    int BitmapMemorySize = Buffer->BytesPerPixel * (Buffer->Width * Buffer->Height);
    Buffer->Memory = VirtualAlloc(0,BitmapMemorySize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
}

internal void
Win32DisplayBufferInWindow(win32_offscreen_buffer *Buffer, 
                           HDC DeviceContext, int WindowWidth, int WindowHeight)
{
    //TODO: Aspect ratio correction.
    
    StretchDIBits(DeviceContext, 
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Buffer->Width, Buffer->Height,
                  Buffer->Memory, 
                  &Buffer->Info,
                  DIB_RGB_COLORS, SRCCOPY);   
}


LRESULT CALLBACK
Win32MainWindowCallback(HWND Window, 
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_SIZE:
        {
            
        } break;
        
        case WM_DESTROY:
        {
            GlobalRunning = 0;
            OutputDebugStringA("WM_DESTROY\n");
        }break;
        
        case WM_CLOSE:
        {
            GlobalRunning = 0;
            OutputDebugStringA("WM_CLOSE\n");
        }break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            // handle input
            b32 IsDown = ((LParam & (1 << 31)) == 0);
            b32 WasDown = ((LParam & (1 << 30)) != 0);
            u32 VKCode = WParam;
            if(IsDown != WasDown)
            {
                if(VKCode == 'W')
                {
                    
                }
                else if (VKCode == 'A')
                {
                    
                }
                else if(VKCode == 'S')
                {
                    
                }
                else if(VKCode == 'D')
                {
                    
                }
                else if (VKCode == 'Q')
                {
                } 
                else if (VKCode == 'E')
                {
                } 
                else if (VKCode == VK_UP)
                {
                } 
                else if (VKCode == VK_DOWN)
                {
                } 
                else if (VKCode == VK_LEFT)
                {
                } 
                else if (VKCode == VK_RIGHT)
                {
                } 
                else if (VKCode == VK_ESCAPE)
                {
                    OutputDebugStringA("ESCAPE: ");
                    if (IsDown)
                    {
                        OutputDebugStringA("IsDown ");
                    }
                    if (WasDown)
                    {
                        OutputDebugStringA("WasDown ");
                    }
                    OutputDebugStringA("\n");
                } 
                else if (VKCode == VK_SPACE)
                {
                }
                b32 AltKeyWasDown = ((LParam & (1 << 29)));
                if((VKCode == VK_F4) && AltKeyWasDown)
                {
                    GlobalRunning = 0;
                }
            }
            
        }
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window,&Paint);
            //do our painting here
            win32_window_dimension Dimension = Win32GetWindowDimension(Window);
            Win32DisplayBufferInWindow(&GlobalBackbuffer,DeviceContext,Dimension.Width,Dimension.Height);
            EndPaint(Window,&Paint);
        }break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    }
    return Result;
}


int CALLBACK 
WinMain(
        HINSTANCE Instance, 
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode
        )
{
    WNDCLASS WindowClass = {0};
    WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    //WindowClass.hIcon;
    WindowClass.lpszClassName = "DungeonHeroClass";
    if(RegisterClassA(&WindowClass))
    {
        HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "DungeonHero",
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      0, 0, Instance, 0);
        if(Window)
        {
            //controller input
            Win32LoadXInput();
            Win32ResizeDIBSection(&GlobalBackbuffer,1280,720);
            // Window creation successful!
            int XOffset = 0;
            int YOffset = 0;
            // NOTE(casey): Since we specified CS_OWNDC,we can just
            // get one device context and use it forever because we are 
            // not sharing it with anyone.
            HDC DeviceContext = GetDC(Window);
            GlobalRunning = 1;
            while(GlobalRunning)// Replaces a for loop
            {
                MSG Message;
                while(PeekMessageA(&Message,0,0,0,PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        GlobalRunning = 0;
                    }
                    //do work in your window
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                //TODO(casey): should we poll this more frequently?
                for(DWORD ControllerIndex = 0;
                    ControllerIndex < XUSER_MAX_COUNT;
                    ++ControllerIndex)
                {
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(ControllerIndex,&ControllerState))
                    {
                        //TODO(casey): see if ControllerState.dwPacketnumber increments too rapidly
                        //NOTE(casey): this controller is plugged in.
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                        int Up            = Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP;
                        int Down          = Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
                        int Left          = Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
                        int Right         = Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
                        int Start         = Pad->wButtons & XINPUT_GAMEPAD_START;
                        int Back          = Pad->wButtons & XINPUT_GAMEPAD_BACK;
                        int LeftShoulder  = Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
                        int RightShoulder = Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
                        int A             = Pad->wButtons & XINPUT_GAMEPAD_A;
                        int B             = Pad->wButtons & XINPUT_GAMEPAD_B;
                        int X             = Pad->wButtons & XINPUT_GAMEPAD_X;
                        int Y             = Pad->wButtons & XINPUT_GAMEPAD_Y;
                        s16 StickX        = Pad->sThumbLX;
                        s16 StickY        = Pad->sThumbLY;
                    }
                    else
                    {
                        //NOTE(casey): This controller is not available
                    }
                    
                }
                RenderWeirdGradient(&GlobalBackbuffer,XOffset,YOffset);
                ++XOffset;
                
                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(&GlobalBackbuffer,DeviceContext,Dimension.Width,Dimension.Height);
            }
        }
        else
        {
            // Window Creation failed! 
            //TODO (hyunjae) : logging
            
        }
    }
    else
    {
        // Window Class Registration failed
    }
    
    
    return (0);
}
