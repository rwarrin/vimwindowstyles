#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline BOOL CALLBACK
FindTopLevelWindow(HWND ThreadWindow, LPARAM LParam)
{
    BOOL Result = 1;
    HWND *OutWindowHandle = (HWND *)LParam;
    *OutWindowHandle = 0;

    if(!GetParent(ThreadWindow))
    {
        *OutWindowHandle = ThreadWindow;
        Result = 0;
    }

    return(Result);
}

static inline HWND
GetTopMostWindow()
{
    HWND Window = 0;

    DWORD ThreadID = GetCurrentThreadId();
    EnumThreadWindows(ThreadID, FindTopLevelWindow, (LPARAM)&Window);

    return(Window);
}

_declspec(dllexport) long
ToggleFullscreen(long Arg)
{
    HWND Window = GetTopMostWindow();
    if(Window)
    {
        DWORD Style = GetWindowLong(Window, GWL_STYLE);
        if(Style & WS_CAPTION)
        {
            MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
            if(GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
            {
                SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(Window, HWND_TOP,
                            MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                            MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                            MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                            SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
        }
        else
        {
            SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
            SendMessage(Window, WM_SYSCOMMAND, SC_RESTORE, 0);
            SendMessage(Window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
    }

    return(GetLastError());
}

_declspec(dllexport) long
ToggleStickyWindow(long Arg)
{
    HWND Window = GetTopMostWindow();
    if(Window)
    {
        DWORD Style = GetWindowLong(Window, GWL_EXSTYLE);
        if(Style & WS_EX_TOPMOST)
        {
            SetWindowPos(Window, HWND_NOTOPMOST,
                         0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
        }
        else
        {
            SetWindowPos(Window, HWND_TOPMOST,
                         0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
        }
    }

    return(GetLastError());
}

_declspec(dllexport) long
SetOpacity(long Arg)
{
    if(Arg < 0)
    {
        Arg = 0;
    }
    else if(Arg > 255)
    {
        Arg = 255;
    }
    unsigned char Opacity = (unsigned char)Arg;

    HWND Window = GetTopMostWindow();
    if(Window)
    {
        DWORD Style = GetWindowLong(Window, GWL_EXSTYLE);
        SetWindowLong(Window, GWL_EXSTYLE, Style | WS_EX_LAYERED);
        SetLayeredWindowAttributes(Window, RGB(0, 0, 0), Opacity, LWA_ALPHA);

        if(Opacity == 255)
        {
            SetWindowLong(Window, GWL_EXSTYLE, Style & ~WS_EX_LAYERED);
        }
    }

    return(GetLastError());
}

#ifdef __cplusplus
}
#endif
