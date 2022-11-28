#include "EventDlg.h"
#include <fstream>
#include <tchar.h>
#include <windowsX.h>
#include <time.h>
#include"resource.h"
#include <Windows.h>
using namespace std;
CEventDlg* CEventDlg::ptr = NULL;

CEventDlg::CEventDlg(void)
{
	ptr = this;
}

CEventDlg::~CEventDlg(void)
{}

void CEventDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

BOOL CEventDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
	hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);
	hEdit3 = GetDlgItem(hwnd, IDC_EDIT3);
	hEdit4 = GetDlgItem(hwnd, IDC_EDIT4);
	hEdit5 = GetDlgItem(hwnd, IDC_EDIT5);
	hEdit6 = GetDlgItem(hwnd, IDC_EDIT6);
	return TRUE;
}

DWORD WINAPI Thread1(LPVOID lp)
{

	srand(time(0));
	HWND hWnd = HWND(lp);
	// получим дескриптор существующего события
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, 0, TEXT("{ECA57A59-2BD7-4fb5-A132-7A00944F7CEF}"));
	if (WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0) {
		ofstream out(TEXT("array.txt"));
		if (!out.is_open())
		{
			return 1;
		}
		int A[100];
		for (int i = 0; i < 100; i++)
		{
			A[i] = rand() % 50;
			out << A[i] << ' ';
		}
		out.close();
		MessageBox(0, TEXT("Поток записал информацию в файл"), TEXT("Критическая секция"), MB_OK);
	}
	return 0;
}

DWORD WINAPI Thread2(LPVOID lp)
{
	srand(time(0));
	HWND hWnd = HWND(lp);
	// получим дескриптор существующего события
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, 0, TEXT("{ECA57A59-2BD7-4fb5-A132-7A00944F7CEF}"));
	ifstream in(TEXT("array.txt"));
	ofstream out(TEXT("result.txt"));
	if (!in.is_open())
	{
		return 1;
	}
	if (WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
	{
		int B[100];
		int sum = 0;
		for (int i = 0; i < 100; i++)
		{
			in >> B[i];
			sum += B[i];
			out << B[i] << ' ';
		}
		in.close();
		out.close();
		MessageBox(0, TEXT("Поток прочитал информацию из файла"), TEXT("Критическая секция"), MB_OK);
		TCHAR str[30];
		wsprintf(str, TEXT("Сумма чисел равна %d"), sum);
		MessageBox(0, str, TEXT("Критическая секция"), MB_OK);
	}
	

	return 0;
}

void CEventDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDC_BUTTON1:
	{
		CreateEvent(NULL, FALSE /*автоматический сброс события */, FALSE /* несигнальное состояние */, TEXT("{ECA57A59-2BD7-4fb5-A132-7A00944F7CEF}"));
		EnableWindow(GetDlgItem(hwnd, IDC_BUTTON1), FALSE);
		HANDLE h;
		h = CreateThread(NULL, 0, Thread2, NULL, 0, NULL);
		CloseHandle(h);
		h = CreateThread(NULL, 0, Thread1, NULL, 0, NULL);
		CloseHandle(h);
	}
	break;
	case IDC_BUTTON2:
	{
		HANDLE h = OpenEvent(EVENT_ALL_ACCESS, 0, TEXT("{ECA57A59-2BD7-4fb5-A132-7A00944F7CEF}"));
		SetEvent(h); // перевод события в сигнальное состояние
	}
	break;

	}
}

BOOL CALLBACK CEventDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
		HANDLE_MSG(hwnd, WM_COMMAND, ptr->Cls_OnCommand);
	}
	return FALSE;
}