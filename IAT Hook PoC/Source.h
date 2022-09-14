#include <iostream>
#include <windows.h>
#include <vector>

#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)


#ifdef __cplusplus
extern "C"
{
#endif

    void DLL_EXPORT GetIAT(uintptr_t Base);

#ifdef __cplusplus
}
#endif