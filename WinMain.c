#include <windows.h>
#define internal static
#define local_persist static
#define global_variable static
global_variable int Running;

LRESULT CALLBACK
MainWindowCallback(HWND Window, 
                   UINT Message,
                   WPARAM WParam,
                   LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
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
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            local_persist DWORD Operation = WHITENESS;
            if(Operation == WHITENESS)
            {
                Operation = BLACKNESS;
            }
            else
            {
                Operation = WHITENESS;
            }
            PatBlt(DeviceContext,X,Y,Width,Height,Operation);
            
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
    WindowClass.lpfnWndProc = MainWindowCallback;
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