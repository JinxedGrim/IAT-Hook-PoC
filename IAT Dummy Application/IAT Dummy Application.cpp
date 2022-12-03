#include <iostream>
#include <windows.h>

typedef int(__stdcall* MsgBox)(HWND, LPCSTR, LPCSTR, UINT);

int main()
{
    MessageBoxA(0, "Loading Dll", "Not Hooked", MB_OK);

    HMODULE Hmod = LoadLibraryA("C:\\Users\\griff\\Desktop\\My Stuff\\My Programming\\IAT Hook PoC\\x64\\Release\\IAT Hook PoC.dll"); // Path to PoC Dll

    if (!Hmod)
    {
        std::cout << "Failed To Load DLL: " << std::endl;
        system("pause");
        return 0;
    }

    while (true)
    {
        MessageBoxA(0, "Not Hooked", "Normal Call", MB_OK);
        MsgBox Msg = (MsgBox)GetProcAddress(GetModuleHandleA("USER32.dll"), "MessageBoxA");
        Msg(0, "Not Hooked", "GetProcAddress", MB_OK);
        Sleep(500);
    }

    return 0;
}