#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include <fstream>
#include <string>

int amountCopies = 3;
CRITICAL_SECTION cs;

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI WriteToFiles(LPVOID lp)
{
    EnterCriticalSection(&cs);

    std::ifstream in("source.txt");
    if (!in) {
        LeaveCriticalSection(&cs);
        return 1;
    }

    std::string line;
    std::string content;

    while (std::getline(in, line)) {
        content += line + "\n";
    }
    in.close();

    for (int i = 0; i < amountCopies; i++)
    {
        char fname[10];
        sprintf_s(fname, "%d.txt", i);
        std::ofstream out(fname);
        if (out) {
            out << content;
            out.close();
        }
    }

    LeaveCriticalSection(&cs);
    return 0;
}

DWORD WINAPI ReadFromFiles(LPVOID lp)
{
    EnterCriticalSection(&cs);

    std::ofstream result("result.txt");
    if (!result) {
        LeaveCriticalSection(&cs);
        return 1;
    }

    for (int i = 0; i < amountCopies; i++)
    {
        char fname[10];
        sprintf_s(fname, "%d.txt", i);
        std::ifstream in(fname);

        if (in) {
            std::string line;
            while (std::getline(in, line)) {
                result << line << "\n";
            }
            in.close();
        }
    }

    result.close();
    LeaveCriticalSection(&cs);
    return 0;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPTSTR lpszCmdLine, int nCmdShow)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        InitializeCriticalSection(&cs);
        return TRUE;
    case WM_CLOSE:
        EndDialog(hWnd, 0);
        DeleteCriticalSection(&cs);
        return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON1)
        {
            CreateThread(NULL, 0, WriteToFiles, 0, 0, NULL);
            CreateThread(NULL, 0, ReadFromFiles, 0, 0, NULL);
            return TRUE;
        }
    }
    return FALSE;
}
