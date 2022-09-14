#include <iostream>
#include <windows.h>

typedef void(*GetIAT_t)(uintptr_t Base);

int main()
{
    HINSTANCE hGetProcIDDLL = LoadLibrary(L"C:\\Users\\griff\\source\\repos\\IAT Hook PoC\\x64\\Release\\IAT Hook PoC.dll"); // Path to PoC Dll

    if (!hGetProcIDDLL)
    {
        std::cout << "Failed To Load DLL" << std::endl;
        system("pause");
        return 0;
    }

    MessageBoxA(0, "Not Hooked", "Not Hooked", MB_OK);

    GetIAT_t GetIAT = (GetIAT_t)GetProcAddress(hGetProcIDDLL, "GetIAT");

    uintptr_t Base = (uintptr_t)GetModuleHandleA("IAT Dummy Application.exe");

    std::cout << "Base: " << "0x" << std::hex << Base << " GetIAT(): 0x" << std::hex << (uintptr_t)GetIAT << std::endl;

    GetIAT(Base);

    while (true)
    {
        MessageBoxA(0, "Not Hooked", "Not Hooked", MB_OK);
    }

    return 0;
}