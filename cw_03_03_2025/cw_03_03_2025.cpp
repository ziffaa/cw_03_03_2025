#include <windows.h>
#include <windowsX.h>
#include <tchar.h>
#include "resource.h"
#include <fstream>
#include <time.h>

HWND hEdit, hCheckT, hCheckB;
HANDLE hThread;

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

CRITICAL_SECTION cs;

DWORD WINAPI WriteText(LPVOID lp)
{
	EnterCriticalSection(&cs);
	std::ofstream out(TEXT("text.txt"));
	int size = GetWindowTextLength(hEdit) + 1;
	TCHAR* buffer = new TCHAR[size];
	GetWindowText(hEdit, buffer, size);
	for (int i = 0; i < size - 1; i++)
	{
		out << (char)buffer[i];
	}
	out.close();
	delete[] buffer;
	MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);
	LeaveCriticalSection(&cs);
	return 0;
}

DWORD WINAPI WriteBinary(LPVOID lp)
{
	EnterCriticalSection(&cs);
	std::ofstream out(TEXT("binary.bin"), std::ios::binary);
	int size = GetWindowTextLength(hEdit) + 1;
	TCHAR* buffer = new TCHAR[size];
	GetWindowText(hEdit, buffer, size);
	for (int i = 0; i < size - 1; i++)
	{
		out << (char)buffer[i];
	}
	out.close();
	delete[] buffer;
	MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);
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
		hEdit = GetDlgItem(hWnd, IDC_EDIT1);
		hCheckT = GetDlgItem(hWnd, IDC_CHECK1);
		hCheckB = GetDlgItem(hWnd, IDC_CHECK2);
		InitializeCriticalSection(&cs);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		DeleteCriticalSection(&cs);
		return TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BUTTON1)
		{
			LRESULT resT = SendMessage(hCheckT, BM_GETCHECK, 0, 0);
			LRESULT resB = SendMessage(hCheckB, BM_GETCHECK, 0, 0);
			if (resT == BST_CHECKED)
			{
				hThread = CreateThread(NULL, 0, WriteText, 0, 0, NULL);
				CloseHandle(hThread);
			}
			if (resB == BST_CHECKED)
			{
				hThread = CreateThread(NULL, 0, WriteBinary, 0, 0, NULL);
				CloseHandle(hThread);
			}
		}
		return TRUE;
	}
	return FALSE;
}
