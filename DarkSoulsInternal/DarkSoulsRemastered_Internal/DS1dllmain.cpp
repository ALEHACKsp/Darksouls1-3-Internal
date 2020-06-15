// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#include "mem.h"
#include "proc.h"

using std::cout;
using std::endl;

std::vector<char> CurAobPos(12);
std::vector<uintptr_t> AOBPos(12);

//void Scan(uintptr_t moduleBase) {
//    coordAddrX = mem::FindDMAAddy(moduleBase + 0x01AA8658, { 0x28 , 0x50 , 0x20 , 0xB8 , 0x8 , 0x80 , 0x120 });
//    coordAddrY = (coordAddrX + 4);
//    coordAddrZ = (coordAddrY + 4);
//
//    x = (float*)coordAddrX;
//    y = (float*)coordAddrY;
//    z = (float*)coordAddrZ;
//}

void Scan(uintptr_t moduleBase) {

    uintptr_t tempAdr = mem::FindDMAAddy(moduleBase + 0x01AA8658, { 0x28 , 0x50 , 0x20 , 0xB8 , 0x8 , 0x80 , 0x120 });
    for (int i = 0; i < 12; i++)
    {
        AOBPos[i] = tempAdr;
        tempAdr += 1;
    }
    
}

DWORD WINAPI main(HMODULE hModule) {

    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);

    cout << "DLL injected, ModuleBase Address = " << std::hex << moduleBase << endl << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n" << endl;
    cout << " Hotkeys \n";
    cout << " 1. Humanity +10                         PAGE_UP \n";
    cout << " 2. Save Coordinate                  INSERT    \n";
    cout << " 3. Restore Coordinate               HOME       \n";
    cout << " 5. Exit                             END       " << endl;

    float s_X = 0;
    float s_Y = 0;
    float s_Z = 0;
    BOOL bStamina = false;

    while (true)
    {

        if (GetAsyncKeyState(VK_END) & 1)
        {
            cout << '\a';
            break;
        }

        if (GetAsyncKeyState(VK_PRIOR) & 1)
        {
            //beep sound
            cout << '\a';
            
            uintptr_t humanityAddr = mem::FindDMAAddy(moduleBase + 0x01AA8658, { 0x0, 0x438, 0x8, 0x7D0, 0x30 , 0x84 });
            int* humanity = (int*)humanityAddr;
            cout << std::hex << humanityAddr << " - Adress of Humanty  \n";
            *humanity = *humanity + 10;

        }

        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            cout << '\a';
            Scan(moduleBase);
            
            for (int i = 0; i < AOBPos.size(); i++)
            {
                char* a = (char*)AOBPos[i];
                CurAobPos[i] = *a;
            }
            cout << "Saved Position.\n";

            //cout << "Current Coords : " << *x << " " << *y << " " << *z << '\a' << endl;
            //s_X = *x;
            //s_Y = *y;
            //s_Z = *z;

        }

        if (GetAsyncKeyState(VK_HOME) & 1)
        {
            cout << '\a';
            Scan(moduleBase);

            //Set invulnerable to avoid damage
            uintptr_t dbgInvulnerableAddr = 0x14036B89E + 0x01982FA4 + 7;
            int* dbgInvulnerable = (int*)dbgInvulnerableAddr;
            *dbgInvulnerable = 1;

            for (int i = 0; i < CurAobPos.size(); i++)
            {
                //set temporary var for addr
                char tempVal = CurAobPos[i];
                char* tempCurAddr = (char*)AOBPos[i];
                 //dereference then set valchar*
                *tempCurAddr = tempVal;
            }
            Sleep(300);
            *dbgInvulnerable = 0;
            cout << "Restored Position \n";

            //Scan(moduleBase);
            ////*x = s_X;
            ////*y = s_Y;
            ////*z = s_Z;
            ////cout << "Restore Coords : " << s_X << " " << s_Y << " " << s_Z << '\a' << endl;

        }

        Sleep(5);
    }


    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//if (GetAsyncKeyState(VK_DELETE) & 1)
//{
//    bStamina = !bStamina;
//    uintptr_t staAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80  , 0x1F90 , 0x18 , 0x1C0 });
//    int* stamina = (int*)staAddr;
//    if (bStamina == true)
//    {
//        cout << '\a' << "Inf Stamina On";
//        *stamina = 17;
//        //cout << std::hex << staAddr << " - Address of stamina " << endl;

//    }
//    else
//    {
//        cout << '\a' << "Inf Stamina Off";
//        *stamina = 0;
//        //cout << std::hex << staAddr << " - Address of stamina " << endl;
//    }

//}