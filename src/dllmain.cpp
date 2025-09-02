#include "pch.h"
#include "MinHook.h"
#include <Windows.h>
#include <cstdio>
#include <cstdarg>


typedef void(__cdecl* conoutf_t)(const char*, ...);
conoutf_t oConoutf = nullptr;


void __cdecl hkConoutf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    if (strstr(fmt, "Total size") != nullptr)
    {

        printf(" [HOOK] hook successful!\n");

        oConoutf("this string hooked!");

    }
    else
    {
        oConoutf(fmt, args);
    }

    va_end(args);
}

DWORD WINAPI MainThread(LPVOID)
{


    AllocConsole();
    FILE* fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    SetConsoleTitle(L"Console");

    printf("dll injected");



    if (MH_Initialize() != MH_OK)
    {
        printf("could not initialize minhook library");
        return 1;
    }

    void* conoutfAddr = (void*)0x0; //conoutf_t function starting address


    if (MH_CreateHook(conoutfAddr, &hkConoutf, reinterpret_cast<LPVOID*>(&oConoutf)) != MH_OK) //hook kurma
    {
        printf("hook not created");
        return 1;
    }

    if (MH_EnableHook(conoutfAddr) != MH_OK)
    {
        printf("hook activation failed");
        return 1;
    }

    printf("conoutf hooked!\n");

    while (true) Sleep(100);


    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, nullptr, 0, nullptr);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


