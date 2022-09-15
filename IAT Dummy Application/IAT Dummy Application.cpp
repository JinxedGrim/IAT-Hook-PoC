#include <iostream>
#include <windows.h>

typedef void(*EmulateInjection_t)();

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
        uintptr_t Address = (uintptr_t)GetProcAddress(Hmod, "TestExport");
        MessageBoxA(0, "Not Hooked", "Not Hooked", MB_OK);
        Sleep(100);
    }

    return 0;
}