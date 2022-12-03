#include "Source.h"

typedef int(__stdcall* MessageBoxA_t)(HWND hwnd, LPCSTR lpText, LPCSTR lpCaption, UINT Utype);
MessageBoxA_t OMessageBoxA = MessageBoxA;
typedef FARPROC(__stdcall* GetProcAddress_t)(HMODULE Hmod, LPCSTR ProcName);
GetProcAddress_t OGetProcAddress = GetProcAddress;

int __stdcall MessageBoxAHk(HWND hwnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    lpText = "Hooked";

    return OMessageBoxA(hwnd, lpText, lpCaption, uType);
}

uintptr_t HookedFunctionAddress = (uintptr_t)MessageBoxAHk;

FARPROC __stdcall GetProcAddressHk(HMODULE Hmod, LPCSTR ProcName)
{
    std::cout << "[+] Imported: " << ProcName << " Function Address: " << std::hex << (uintptr_t)OGetProcAddress(Hmod, ProcName) << " ModBase: " << (uintptr_t)Hmod << std::endl;

    if (strcmp(ProcName, "MessageBoxA") == 0)
    {
        return (FARPROC)MessageBoxAHk;
    }

    return OGetProcAddress(Hmod, ProcName);
}

uintptr_t HookedGetProcAddress = (uintptr_t)GetProcAddressHk;

DWORD WINAPI GetIAT(LPVOID Param)
{
    PeParser PeParse = PeParser((uintptr_t)GetModuleHandleA("IAT Dummy Application.exe"));

    if (PeParse.InitHeaders())
    {
        std::cout << "[+] PE Is Valid" << std::endl;
    }
    else
    {
        std::cout << "[!] Pe Isn't Valid" << std::endl;
        system("pause");
        return NULL;
    }

    uintptr_t ImportRVA = PeParse.GetOptionalDataDirectoryRVA(IMAGE_DIRECTORY_ENTRY_IMPORT);

    PIMAGE_THUNK_DATA MsgBoxThunkData = PeParse.GetFunctionThunkByName(PeParse.GetImportDescriptorByName("USER32.dll", ImportRVA), "MessageBoxA", false);
    PIMAGE_THUNK_DATA GetProcThunkData = PeParse.GetFunctionThunkByName(PeParse.GetImportDescriptorByName("KERNEL32.dll", ImportRVA), "GetProcAddress", false);


    if (!MsgBoxThunkData->u1.Function || !GetProcThunkData->u1.Function)
    {
        std::cout << "[!] Failed To Get Function Thunk Data" << std::endl;
        system("pause");
        return NULL;
    }

    uintptr_t OrigImportAddress = MsgBoxThunkData->u1.Function;
    uintptr_t OrigGetProcAddress = GetProcThunkData->u1.Function;

    std::cout << "[+] Found MsgBox Import Address: 0x" << std::hex << OrigImportAddress << std::endl;
    std::cout << "[+] Replacing With Hooked Function: " << HookedFunctionAddress << std::endl;

    DWORD OldProtect;
    DWORD OldProtect2;

    VirtualProtect((void*)(&MsgBoxThunkData->u1.Function), 8, PAGE_EXECUTE_READWRITE, &OldProtect);
    MsgBoxThunkData->u1.Function = HookedFunctionAddress;
    VirtualProtect((void*)(&MsgBoxThunkData->u1.Function), 8, OldProtect, &OldProtect);

    VirtualProtect((void*)(&GetProcThunkData->u1.Function), 8, PAGE_EXECUTE_READWRITE, &OldProtect2);
    GetProcThunkData->u1.Function = HookedGetProcAddress;
    VirtualProtect((void*)(&GetProcThunkData->u1.Function), 8, OldProtect2, &OldProtect2);
    
    while (!GetAsyncKeyState(VK_F7))
    {
        Sleep(1000);
    }

    std::cout << "[+] Uninstalling Hooks" << std::endl;


    VirtualProtect((void*)(&MsgBoxThunkData->u1.Function), 8, PAGE_EXECUTE_READWRITE, &OldProtect);
    MsgBoxThunkData->u1.Function = OrigImportAddress;
    VirtualProtect((void*)(&MsgBoxThunkData->u1.Function), 8, OldProtect, &OldProtect);

    VirtualProtect((void*)(&GetProcThunkData->u1.Function), 8, PAGE_EXECUTE_READWRITE, &OldProtect2);
    GetProcThunkData->u1.Function = OrigGetProcAddress;
    VirtualProtect((void*)(&GetProcThunkData->u1.Function), 8, OldProtect2, &OldProtect2);


    std::cout << "[+] Hooks Uninstalled\n" << std::endl;

    FreeLibraryAndExitThread(GetModuleHandleA("IAT Hook PoC.dll"), 0x0);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)GetIAT, 0, 0, 0);

    case DLL_PROCESS_DETACH:
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}