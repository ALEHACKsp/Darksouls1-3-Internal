// dllmain.cpp : Defines the entry point for the DLL application.

#include "pch.h"
#include "mem.h"
#include "proc.h"


using std::cout;
using std::endl;
uintptr_t moduleBase = (uintptr_t)GetModuleHandle(NULL);
// initialize vars
std::vector<char> CurAobPos(12);
std::vector<uintptr_t> AOBPos(12);
bool bRegen = false;
bool threadClose = false;

void DisplayMenu() {
    //CMD display
    cout << "DLL injected, ModuleBase Address = " << std::hex << moduleBase << endl << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ \n" << endl;
    cout << " Hotkeys                                   \n";
    cout << " Clear Menu                          F4 " << endl;
    cout << " 1. Inf Stamina                      DEL   "<< endl;
    cout << " 2. Save Coordinate                  INSERT "<< endl;
    cout << " 3. Restore Coordinate               HOME       " << endl;
    cout << " 4. Regend MP & HP (5% per sec)       +         " << endl;
    cout << " 5. Speed Modifier                  PAGE_UP         " << endl;
    cout << " 6. Exit                             END       " << endl;
}

// Scan for postion(x,y,z) AOB addr and store to vector
void Scan(uintptr_t moduleBase) {

    uintptr_t tempAdr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x40 , 0x28 , 0x80});
    for (int i = 0; i < 12; i++)
    {
        AOBPos[i] = tempAdr;
        tempAdr += 1;
    }

}

// fix dll inject , reinject failed = failed to close thread;
void regenFunc(float percent) {
    uintptr_t hpAddr = 0, maxHpAddr = 0, mpAddr = 0, maxMpAddr = 0, baseAddr = 0;
    while (true)
    {
        if (threadClose)
        {
            break;
        }

        //FP & HP Regen
        if (bRegen==true)
        {
            baseAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80 ,0x1F90 ,0x18 ,0});
            // check if pointer is valid
            if (baseAddr == 0)
            {
                cout << "Bad Pointer \n";
            }
            else
            {
                maxHpAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80 ,0x1F90 ,0x18 ,0xDC });
                hpAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80 ,0x1F90 ,0x18 ,0xD8 });
                mpAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80 ,0x1F90 ,0x18 ,0xE4 });
                maxMpAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80 ,0x1F90 ,0x18 ,0xE8 });
                int* hp = (int*)hpAddr;
                int* mp = (int*)mpAddr;
                int* maxHP = (int*)maxHpAddr;
                int* maxMP = (int*)maxMpAddr;
                if (*hp < *maxHP)
                    *hp += percent * *maxHP / 100;

                if (*mp < *maxMP)
                    *mp += percent * *maxMP / 100;
            }

        }

        Sleep(1000);
    }
}

DWORD WINAPI main(HMODULE hModule) {

    AllocConsole();
    FILE* f;
    freopen_s(&f,"CONOUT$", "w", stdout);

    DisplayMenu();                                  //Display Hack menu
    float s_X = 0, s_Y = 0 , s_Z = 0;                              // temp var for POS
    float regen = 2.0;                            // regen percent per max hp & mp
    BOOL bStamina = false;
    std::thread thread_obj(regenFunc,regen);          // create new thread for regen function
    while (true)
    {
        //Exit
        if (GetAsyncKeyState(VK_END) & 1)
        {
            cout << '\a';
            break;
        }
        
        //Reset Display of hack menu
        if (GetAsyncKeyState(VK_F4))
        {
            fclose(f);
            FreeConsole();
            AllocConsole();
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            DisplayMenu();
        }

        //Toggle Stamina
        if (GetAsyncKeyState(VK_DELETE) & 1)
        {
            bStamina = !bStamina;
            uintptr_t staAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80  , 0x1F90 , 0x18 , 0x1C0 });
            //validate pointer
            if (staAddr == 0)
            {
                cout << "Bad PTR \n";

            }
            else
            {
                int* stamina = (int*)staAddr;
                if (bStamina == true)
                {
                    cout << '\a' << "Inf Stamina On" << endl;
                    *stamina = 17;

                }
                else
                {
                    cout << '\a' << "Inf Stamina Off" << endl;
                    *stamina = 0;

                }
            }
        }

        //Toggle FP & HP Regen
        if (GetAsyncKeyState(VK_OEM_PLUS) & 1)
        {
            cout << "regen =" << bRegen << endl;
            bRegen = !bRegen;

        }


        //Speed up
        if (GetAsyncKeyState(VK_PRIOR) & 1)
        {
            uintptr_t spdAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80  , 0x1F90 , 0x28 , 0xA58 });
            //spdAddr pointer
            if (spdAddr == 0)
            {
                cout << "Bad PTR \n";

            }
            else
            {
                float* speed = (float*)spdAddr;
                *speed = *speed + 1.0f;
                cout << '\a' << "Speed = " << std::dec << *speed << endl;

            }
        }

        //Speed down
        if (GetAsyncKeyState(VK_NEXT) & 1)
        {
            uintptr_t spdAddr = mem::FindDMAAddy(moduleBase + 0x4768E78, { 0x80  , 0x1F90 , 0x28 , 0xA58 });
            //spdAddr pointer
            if (spdAddr == 0)
            {
                cout << "Bad PTR \n";

            }
            else
            {
                float* speed = (float*)spdAddr;
                if (*speed != 1.0f)
                {
                    *speed = *speed - 1.0f;
                    cout << '\a' << "Speed = " << std::dec << *speed << endl;

                }
            }
        }

        //Save POS
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

        }

        //Restore POS
        if (GetAsyncKeyState(VK_HOME) & 1)
        {
            cout << '\a';
            Scan(moduleBase);
            //Set invulnerable to avoid damage
            uintptr_t dbgInvulnerableAddr = moduleBase + 0x4768F68 + 8;
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
            Sleep(100);
            *dbgInvulnerable = 0;
            cout << "Restored Position \n";

        }


        Sleep(5);

    }
    // close regen thread
    threadClose = true;
    Sleep(1000);
    thread_obj.detach();
    thread_obj.~thread();
    //clean memory
    fclose(f);
    FreeConsole();
    //close main thread
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
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


//uintptr_t humanityAddr = mem::FindDMAAddy(moduleBase + 0x01AA8658, { 0x0, 0x438, 0x8, 0x7D0, 0x30 , 0x84 });
//int* humanity = (int*)humanityAddr;
//cout << std::hex << humanityAddr << " - Adress of Humanty  \n";
//*humanity = *humanity + 10;