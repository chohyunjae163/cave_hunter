#include <windows.h>
#include <stdint.h>

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

#define internal static
#define local_persist static
#define global_variable static

global_variable int Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory; //a block memory for the purpose of drawing pixels into.
global_variable int BitmapWidth;
global_variable int BitmapHeight;

internal void
Win32ResizeDIBSection(int Width, int Height)
{
    BitmapWidth = Width;
    BitmapHeight = Height;
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight; //negative value: top-down pitch
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory,0,MEM_RELEASE);
    }
    
    int BytesPerPixel = 4;
    int BitmapMemorySize = BytesPerPixel * (BitmapWidth * BitmapHeight);
    BitmapMemory = VirtualAlloc(0,BitmapMemorySize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    int Pitch = Width * BytesPerPixel;
    u8 *Row = (u8 *)BitmapMemory;
    for(int Y = 0;
        Y < BitmapHeight;
        ++Y)
    {
        u32 *Pixel = (u32 *)Row;
        for(int X = 0;
            X < BitmapWidth;
            ++X)
        {
            //write color to pixel
            ++Pixel;
        }
        Row +=Pitch;
    }
    
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect)
{
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    
    StretchDIBits(DeviceContext, 
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, BitmapWidth, BitmapHeight,
                  BitmapMemory, 
                  &BitmapInfo,
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
            RECT ClientRect;
            GetClientRect(Window,&ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
        } break;
        
        case WM_DESTROY:
        {
            Running = 0;
            OutputDebugStringA("WM_DESTROY\n");
        }break;
        
        case WM_CLOSE:
        {
            Running = 0;
            OutputDebugStringA("WM_CLOSE\n");
        }break;
        
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window,&Paint);
            //do our painting here
            RECT ClientRect;
            GetClientRect(Window,&ClientRect);
            Win32UpdateWindow(DeviceContext,&ClientRect);
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
        HWND Window = CreateWindowExA(0, WindowClass.lpszClassName, "Handmade Hero",
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      0, 0, Instance, 0);
        if(Window)
        {
            // Window creation successful!
            Running = 1;
            while(Running)// Replaces a for loop
            {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message,0,0,0);
                if(MessageResult > 0) // 0 is the WM_QUIT message, -1 is invalid window handle
                {
                    //do work in your window
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                else
                {
                    break; //break out of the loop
                }
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
