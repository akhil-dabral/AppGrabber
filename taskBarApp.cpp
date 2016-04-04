// taskBarApp.cpp : Defines the entry point for the application.


#include <vld.h>
#include "stdafx.h"
#include "commctrl.h"
#include "taskBarApp.h"
#include<windows.h>
#include<string.h>
#include<iostream>
#include<CommCtrl.h>
#include<iostream>
#include<list>
#include<vector>
#include<Psapi.h>
#include<Shellapi.h>
#include<windowsx.h>
#include "WinEventListener/WinEventListener.h"
//#define _WIN32_WINNT 0x0500
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "WinEventListener/Debug/WinEventListener.lib")

int wmId, wmEvent;
PAINTSTRUCT ps;
HDC hdc;
RECT rect;
HWND newhWnd;


#pragma comment(lib,"User32.lib")


#define MAX_LOADSTRING 100
#define WM_MYBUTTON 4353
#define ID_PORT_BUTTON 546


struct taskData {
    HDESK desktophWnd;
    LPARAM WinHandle;
    TCHAR path[4096];
    HICON hIcon;
     std::list < std::list < struct taskData *>>::iterator subTaskListItrat;

};





// Global Variables:
HINSTANCE hInst;		// current instance
TCHAR szTitle[MAX_LOADSTRING];	// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];	// the main window class name
HIMAGELIST g_hImageList = NULL;
static UINT WM_SHELLHOOKMESSAGE;
HWND hWndEdit = NULL;
HWND hWndToolbar = NULL;
HWND hWnd = NULL;





std::list < struct taskData *>listNode;
std::list < struct taskData *>::iterator itrat;

std::list < std::list < struct taskData *>>subTaskList;
std::list < std::list < struct taskData *>>::iterator subTaskListItrat;


std::list < struct taskData *>::iterator subItrat;



std::list < TCHAR * >desktopList;
std::list < TCHAR * >::iterator i;




// Forward declarations of functions included in this code module:
LRESULT CALLBACK EditControlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI CallWndProc(int, WPARAM, LPARAM);
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
BOOL SetShellHook(HWND hwndHook);
BOOL UnSetShellHook(HWND hwndHook);
VOID AddToList(HWND hwndMainWnd, LPARAM hwnd, LPCSTR szMsg);
VOID RemoveFromList(HWND hwndMainWnd, LPARAM hwnd, LPCSTR szMsg);
void freeList();
void addList(LPARAM hwnd, HDESK desktophWnd);
BOOL CALLBACK EnumDesktopProc(LPTSTR lpszDesktop, LPARAM lParam);
void MySystemProc(HWND);
BOOL checkList(LPTSTR);
BOOL BuildToolbar(int flag);
BOOL CreateSimpleToolbar();
BOOL Search(HWND editHwnd);
BOOL CALLBACK EnumChildProc(HWND childhWnd, LPARAM lParam);

//++++++++++++++++++++ PROGRAMS MAIN ++++++++++++++++++++++++++++++++++++++++++++++++++++++

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_TASKBARAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow)) {
	return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASKBARAPP));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0)) {
	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }

    return (int) msg.wParam;
}

//++++++++++++++++++++ Register Window Class ++++++++++++++++++++++++++++++++++++++++++++++++++++++

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;



    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASKBARAPP));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL;//(HBRUSH) (COLOR_MENU);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MAINFRAME);;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//++++++++++++++++++++ Init Main Window ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

    BOOL ret;



    hInst = hInstance;		// Store instance handle in our global variable

    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN) - 23;


    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    SetWindowPos(hWnd, HWND_TOP, 0, 0, 800, 800, SWP_NOREDRAW | SWP_SHOWWINDOW);
    //SetWindowPos(hWnd, HWND_TOPMOST,0,0,height,width, SWP_NOREDRAW | SWP_SHOWWINDOW);

    if (!hWnd) {
	return FALSE;
    }

    if (!SetShellHook(hWnd)) {
	MessageBox(hWnd, TEXT("F"), NULL, MB_OK);
	return 3;
    }

    MySystemProc(hWnd);   // Enumerate Windows and Desktops

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//++++++++++++++++++++ Windows Proc ++++++++++++++++++++++++++++++++++++++++++++++++++++++

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    LPNMTOOLBAR lPEvent;
    LPNMTBGETINFOTIP lptbgit;
    LPNMMOUSE lpnm;
    LPNMTBDISPINFO lptbdi;
    LPNMKEY lpnmk;
    HDESK a, b, z;
    DWORD tid;
    DWORD pid;


	//--------------------- Windows Hook Events--------------------

    if (message == WM_SHELLHOOKMESSAGE) {
	switch (LOWORD(wParam)) {
	case HSHELL_WINDOWCREATED:
	    AddToList(hWnd, lParam, "HSHELL_WINDOWCREATED");
	    break;
	case HSHELL_WINDOWDESTROYED:
	    RemoveFromList(hWnd, lParam, "HSHELL_WINDOWDESTROYED");
	    break;
	}
	return 0;
    }
	//---------------------------------------------------------------

    switch (message) {


    case WM_MYBUTTON:
    case WM_COMMAND:
	wmId = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	// Parse the menu selections:
	newhWnd = (HWND) lParam;
	lPEvent = (LPNMTOOLBAR) lParam;
	lptbgit = (LPNMTBGETINFOTIP) lParam;
	lptbdi = (LPNMTBDISPINFO) lParam;
	lpnm = (LPNMMOUSE) lParam;

	if (HIWORD(wParam) == EN_CHANGE) {
	    Search((HWND) lParam);
	}

	switch (wmId) {

	case ID_PORT_BUTTON:								// When Button Pressed
	    OutputDebugString(L"Button Pressed \n");

	    break;
	case IDM_ABOUT:
	    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
	    break;
	case IDM_EXIT:
	    DestroyWindow(hWnd);
	    break;
	default:
	    return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
    case WM_PAINT:
	GetClientRect(hWnd, &rect);
	hdc = BeginPaint(hWnd, &ps);
	// TODO: Add any drawing code here...
	OutputDebugStringW(L"In Paint");

	EndPaint(hWnd, &ps);
	break;
    case WM_DESTROY:
	freeList();
	ImageList_Remove(g_hImageList, -1);
	ImageList_Destroy(g_hImageList);
	DestroyWindow(hWndToolbar);
	UnSetShellHook(hWnd);
	PostQuitMessage(0);
	break;
    case WM_NOTIFY:											// Capture Notification Messages

	lptbgit = (LPNMTBGETINFOTIP) lParam;
	lPEvent = (LPNMTOOLBAR) lParam;
	lpnm = (LPNMMOUSE) lParam;
	lptbdi = (LPNMTBDISPINFO) lParam;
	newhWnd = (HWND) lParam;
	lpnmk = (LPNMKEY) lParam;



	TCHAR szText[0x1000];
	switch (lPEvent->hdr.code) {
	case TBN_DROPDOWN:										//When Dropdown Button Pressed
	    OutputDebugString(L"Dropdown Pressed \n");
	    {
		// Get the coordinates of the button.
		RECT rc;
		if (!SendMessage(lPEvent->hdr.hwndFrom, TB_GETRECT, (WPARAM) lPEvent->iItem, (LPARAM) & rc))
		    OutputDebugString(L"Error get rect");

		// Convert to screen coordinates.            
		MapWindowPoints(lPEvent->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT) & rc, 2);

		// Get the menu.
		HMENU hMenuLoaded = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU2));

		HICON hIcon = (HICON) GetClassLong((HWND) lPEvent->tbButton.iBitmap, GCL_HICON);

		GetWindowText(((HWND) lPEvent->tbButton.dwData), szText, sizeof(szText));
		// Get the submenu for the first menu item.
		HMENU hPopupMenu = GetSubMenu(hMenuLoaded, 0);
		AppendMenu(hPopupMenu, MF_STRING, 0, szText);




		TPMPARAMS tpm;

		tpm.cbSize = sizeof(TPMPARAMS);
		tpm.rcExclude = rc;




		TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, hWnd, &tpm);



		return TRUE;

	    }
	    break;
	    //case TBN_GETINFOTIP : 


	default:
	    break;
	}
	switch (lptbgit->hdr.code) {
	case TBN_GETINFOTIP:
	    OutputDebugString(TEXT("ToolTip Info \n"));
	    break;
	default:
	    break;
	}
	switch (lPEvent->hdr.code) {
		 HDESK b;

	case NM_CLICK:											// When Taskbar Button Pressed
	    OutputDebugString(L"ButtonPressed ");
		 b = GetThreadDesktop(GetCurrentThreadId());
	   /* tid = GetWindowThreadProcessId((HWND) lPEvent->tbButton.iBitmap, &pid);
	    a = GetThreadDesktop(tid);
	    b = GetThreadDesktop(GetCurrentThreadId());
	    if (GetThreadDesktop(tid) != GetThreadDesktop(GetCurrentThreadId())) {
		SwitchDesktop((HDESK) a);
		ShowWindow(((HWND) lPEvent->tbButton.iBitmap), SW_RESTORE);
		SetForegroundWindow((HWND) lPEvent->tbButton.iBitmap);
		break;
	    }

	    if (IsIconic(((HWND) lPEvent->tbButton.iBitmap)))
		ShowWindow(((HWND) lPEvent->tbButton.iBitmap), SW_RESTORE);
	    else
		ShowWindow(((HWND) lPEvent->tbButton.iBitmap), SW_MINIMIZE);

		*/
		if(GetParent((HWND) lPEvent->tbButton.iBitmap) != hWnd)
		SetParent((HWND) lPEvent->tbButton.iBitmap,hWnd);
		else
			SetParent((HWND) lPEvent->tbButton.iBitmap,NULL);
	    break;


	default:
	    break;
	}
	


	break;


    case WM_CREATE:
	break;




    default:

	return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
	return (INT_PTR) TRUE;

    case WM_COMMAND:
	if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
	    EndDialog(hDlg, LOWORD(wParam));
	    return (INT_PTR) TRUE;
	}
	break;
    }
    return (INT_PTR) FALSE;
}


//++++++++++++++++++++ Design Toolbar  ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CreateSimpleToolbar()
{

    // Declare and initialize local constants.
    const int ImageListID = 1;

    const int bitmapSize = 24;

    TBBUTTON tbb, spacer;
    int a;
    TBBUTTONINFO tbBtnInfo;
    TBBUTTON tbButton;



    DWORD buttonStyle, command;





    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM) ImageListID, (LPARAM) g_hImageList);

    TCHAR szText[0x100] = { 0 };
    // Set the image list.
    itrat = listNode.begin();
    while (itrat != listNode.end()) {




	a = ImageList_AddIcon(g_hImageList, (*itrat)->hIcon);
	if (a < 0)		// Sets Default Icon if icon not found
	    a = ImageList_AddIcon(g_hImageList, LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)));


	GetWindowText((HWND) ((*itrat)->WinHandle), szText, sizeof(szText));



	HINSTANCE Wininstance = (HINSTANCE) GetWindowLongPtr((HWND) ((*itrat)->WinHandle), GWLP_HINSTANCE);
	if ((*itrat)->subTaskListItrat->size() == 1) {
	    buttonStyle = TBSTYLE_BUTTON;
	    command = ID_PORT_BUTTON;

	} else {
	    buttonStyle = TBSTYLE_DROPDOWN;
	    command = ID_PORT_BUTTON;

	}


	tbButton.fsState = TBSTATE_ENABLED;
	tbButton.fsStyle = buttonStyle;
	tbButton.iBitmap = a;
	tbButton.idCommand = command;
	tbButton.iString = SendMessage(hWndToolbar, TB_ADDSTRING, NULL, (LPARAM) (szText));
	tbButton.dwData = ((*itrat)->WinHandle);

	tbb.iBitmap = 0;
	tbb.fsState = TBSTATE_ENABLED;
	tbb.fsStyle = BTNS_SEP;
	tbb.idCommand = 0;	// Sent in WM_COMMAND when pressed                                              // Sent in WM_COMMAND when pressed
	tbb.iString = 0;


	spacer.iBitmap = 0;
	spacer.fsState = TBSTATE_ENABLED;
	spacer.fsStyle = BTNS_SEP;
	spacer.idCommand = 0;	// Sent in WM_COMMAND when pressed                                              // Sent in WM_COMMAND when pressed
	spacer.iString = 0;




	tbBtnInfo.cbSize = sizeof(TBBUTTONINFO);
	tbBtnInfo.cchText = sizeof(szText);
	tbBtnInfo.cx = 24;
	tbBtnInfo.dwMask = TBIF_LPARAM | TBIF_IMAGE;
	tbBtnInfo.fsState = TBSTATE_ENABLED;
	tbBtnInfo.fsStyle = buttonStyle;
	tbBtnInfo.idCommand = command;
	tbBtnInfo.iImage = a;
	tbBtnInfo.lParam = ((*itrat)->WinHandle);
	tbBtnInfo.pszText = szText;

	SendMessage(hWndToolbar, TB_SETPADDING, 0, MAKELONG(0, 15));



	SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
	SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);

	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(tbButton), 0);



	SendMessage(hWndToolbar, TB_INSERTBUTTON, (WPARAM) 1, (LPARAM) & tbb);
	SendMessage(hWndToolbar, TB_INSERTBUTTON, (WPARAM) 1, (LPARAM) & tbButton);

	if (!SendMessage(hWndToolbar, TB_SETBUTTONINFO, (WPARAM) ID_PORT_BUTTON, (LPARAM) & tbBtnInfo))
	    OutputDebugString(L"ERRORRRRRRRRR");

	SendMessage(hWndToolbar, TB_SETBUTTONSIZE, NULL, MAKELONG(40, 36));



	itrat++;
    }
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    ShowWindow(hWndToolbar, TRUE);
    ShowWindow(hWndEdit, TRUE);
    return 1;
}

//++++++++++++++++++++ Callback function for Enumrate Running Application ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char WinText[256] = { NULL };
    HICON hIcon;
    HINSTANCE hInstance;
    TCHAR stringTitle[1000] = { NULL };
    HDESK desktophWnd = (HDESK) lParam;
    int length;

    TCHAR start[] = TEXT("Start");
    TCHAR program[] = TEXT("Program Manager");
    TCHAR taskbar[] = TEXT("taskBarApp");
    TCHAR dcMonitor[] = TEXT("DataCardMonitor");

    DWORD pid = 0;
    WORD icon = 0;


    length = GetWindowText(hwnd, stringTitle, sizeof(stringTitle));

    if (lstrcmp(stringTitle, (LPCWSTR) "") && (IsWindowVisible(hwnd))) {
	if (!lstrcmp(stringTitle, (LPCWSTR) start) || !lstrcmp(stringTitle, (LPCWSTR) program) || !lstrcmp(stringTitle, (LPCWSTR) taskbar) || !lstrcmp(stringTitle, (LPCWSTR) dcMonitor))
	    return TRUE;


	GetWindowTextA(hwnd, WinText, sizeof(WinText));

	OutputDebugStringW(L"Window");

	OutputDebugStringW(L" n");

	addList((LPARAM) hwnd, desktophWnd);

	return TRUE;
    }




    return TRUE;		// Continue Recursion
}

//++++++++++++++++++++ Register Shell Hook ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL SetShellHook(HWND hwndHook)
{
    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(TEXT("SHELLHOOK"));
    if (WM_SHELLHOOKMESSAGE) {
	if (RegisterShellHookWindow(hwndHook)) {
	    return TRUE;
	}
    }
    return FALSE;
}

//++++++++++++++++++++Destroy Shell Hook++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL UnSetShellHook(HWND hwndHook)
{
    if (DeregisterShellHookWindow(hwndHook)) {
	return TRUE;
    } else {
	return FALSE;
    }
}

//++++++++++++++++++++ Call When New Application Launches ++++++++++++++++++++++++++++++++++++++++++++++++++++++

VOID AddToList(HWND hwndMainWnd, LPARAM hwnd, LPCSTR szMsg)
{
    HICON hIcon;
    WORD icon = 0;
    TCHAR szCls[0x1000] = { 0 };
    TCHAR szText[0x1000] = { 0 };
    TCHAR temp[0x80] = { 0 };
    TCHAR buf[0x100] = { 0 };
    TCHAR stringTitle[1000] = { NULL };
    int length;
    BOOL flag = 1;
    HDESK desktophWnd = GetThreadDesktop(GetCurrentThreadId());
    DWORD pid = 0;
    TCHAR tab[] = TEXT("TabWindowClass");

    std::list < struct taskData *>newList;
    length = GetWindowText((HWND) hwnd, stringTitle, sizeof(stringTitle));

    GetWindowText((HWND) hwnd, szText, sizeof(szText));
    GetClassName((HWND) hwnd, szCls, sizeof(szCls));
    TCHAR filename[1000] = { NULL };
    GetWindowThreadProcessId((HWND) hwnd, &pid);
    HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    GetModuleFileNameEx(proc, NULL, filename, sizeof(filename));

    HINSTANCE Wininstance = (HINSTANCE) GetWindowLongPtr((HWND) hwnd, GWLP_HINSTANCE);

    hIcon = ExtractAssociatedIcon(Wininstance, filename, &icon);

    GetWindowText((HWND) hwnd, szText, sizeof(szText));
    GetClassName((HWND) hwnd, szCls, sizeof(szCls));
    static HWND hwndEdit = GetDlgItem(hwndMainWnd, 101);
    //&& wcscmp(szCls,TEXT("IME"))
    UINT len = GetWindowTextLength(hwndEdit);
    OutputDebugString(L"Ïn AddtoListn");

    if ((lstrcmp(stringTitle, (LPCWSTR) "") && (IsWindowVisible((HWND) hwnd))) || !lstrcmp(szCls, tab)) {
	OutputDebugString(L"TAB OPPEN n");
	itrat = listNode.begin();
	while (itrat != listNode.end()) {

	    if (wcsstr((*itrat)->path, TEXT("iexplore.exe"))) {
		subItrat = (*itrat)->subTaskListItrat->begin();
		while (subItrat != (*itrat)->subTaskListItrat->end()) {

		    free(*subItrat);
		    subItrat++;

		}
		(*itrat)->subTaskListItrat->clear();
		EnumChildWindows((HWND) (*itrat)->WinHandle, EnumChildProc, (*itrat)->WinHandle);
	    }

	    if (!wcscmp((*itrat)->path, filename)) {
		struct taskData *newNode = (struct taskData *) malloc(sizeof(struct taskData));
		newNode->desktophWnd = desktophWnd;
		newNode->WinHandle = hwnd;
		wcscpy(newNode->path, filename);
		newNode->hIcon = hIcon;
		flag = 0;
		(*itrat)->subTaskListItrat->push_back(newNode);



	    }

	    itrat++;
	}
	if (flag) {
	    struct taskData *newNode = (struct taskData *) malloc(sizeof(struct taskData));
	    struct taskData *newSubNode = (struct taskData *) malloc(sizeof(struct taskData));

	    newSubNode->desktophWnd = desktophWnd;
	    newSubNode->WinHandle = hwnd;
	    wcscpy(newSubNode->path, filename);
	    newSubNode->hIcon = hIcon;
	    newList.push_back(newSubNode);
	    subTaskList.push_front(newList);

	    subTaskListItrat = subTaskList.begin();

	    newNode->desktophWnd = desktophWnd;
	    newNode->WinHandle = hwnd;
	    wcscpy(newNode->path, filename);
	    newNode->hIcon = hIcon;
	    newNode->subTaskListItrat = subTaskListItrat;

	    listNode.push_front(newNode);





	}





    }
    CloseHandle(proc);
    ImageList_Remove(g_hImageList, -1);
    ImageList_Destroy(g_hImageList);
    DestroyWindow(hWndToolbar);
    BuildToolbar(1);
    CreateSimpleToolbar();
    return;
}

//++++++++++++++++++++ Call When Application Closed ++++++++++++++++++++++++++++++++++++++++++++++++++++++

VOID RemoveFromList(HWND hwndMainWnd, LPARAM hwnd, LPCSTR szMsg)
{
    DWORD pid = 0;
    TCHAR filename[1000] = { NULL };
    int flag = 0, counter = 0, loopchk = 0;
    int length;
    TCHAR stringTitle[1000] = { NULL };
    TCHAR szCls[100] = { 0 };

    std::list < struct taskData *>::iterator subItrat;

    GetWindowThreadProcessId((HWND) hwnd, &pid);
    HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    GetModuleFileNameEx(proc, NULL, filename, sizeof(filename));
    length = GetWindowText((HWND) hwnd, stringTitle, sizeof(stringTitle));

    GetClassName((HWND) hwnd, szCls, sizeof(szCls));

    if (lstrcmp(stringTitle, (LPCWSTR) "")) {
	itrat = listNode.begin();
	while (itrat != listNode.end()) {

	    if (!wcscmp((*itrat)->path, filename)) {

		if (wcsstr(filename, TEXT("iexplore.exe"))) {
		    if ((*itrat)->WinHandle == hwnd) {
			subItrat = (*itrat)->subTaskListItrat->begin();
			while (subItrat != (*itrat)->subTaskListItrat->end()) {

			    free(*subItrat);
			    subItrat++;

			}
			(*itrat)->subTaskListItrat->clear();
			subTaskList.remove(*(*itrat)->subTaskListItrat);
			free((*itrat));
			listNode.remove(*itrat);
			flag = 1;
			break;

		    } else {
			subItrat = (*itrat)->subTaskListItrat->begin();
			while (subItrat != (*itrat)->subTaskListItrat->end()) {

			    free(*subItrat);
			    subItrat++;

			}
			(*itrat)->subTaskListItrat->clear();
			EnumChildWindows((HWND) (*itrat)->WinHandle, EnumChildProc, (*itrat)->WinHandle);
			flag = 0;
			break;

		    }

		}
		subItrat = (*itrat)->subTaskListItrat->begin();
		while (subItrat != (*itrat)->subTaskListItrat->end()) {

		    if ((*subItrat)->WinHandle == hwnd) {

			(*itrat)->hIcon = (*subItrat)->hIcon;
			(*itrat)->WinHandle = (*subItrat)->WinHandle;

			free(*subItrat);
			if ((*itrat)->subTaskListItrat->size() == 1) {

			    (*itrat)->subTaskListItrat->remove(*subItrat);
			    subTaskList.remove(*(*itrat)->subTaskListItrat);

			    free((*itrat));
			    listNode.remove(*itrat);


			    flag = 1;
			    loopchk = 1;
			    OutputDebugString(L"Deleted");
			    break;
			} else {

			    (*itrat)->subTaskListItrat->remove(*subItrat);

			    flag = 1;
			    loopchk = 1;
			    OutputDebugString(L"Deleted");
			}
			break;
		    }


		    subItrat++;
		}


	    }
	    if (loopchk)
		break;


	    itrat++;
	}
	if (flag == 1) {
	    CloseHandle(proc);
	    ImageList_Remove(g_hImageList, -1);
	    ImageList_Destroy(g_hImageList);
	    DestroyWindow(hWndToolbar);
	    BuildToolbar(1);
	    CreateSimpleToolbar();
	}

    }
}

//++++++++++++++++++++ Clean The Complete Data Structures ++++++++++++++++++++++++++++++++++++++++++++++++++++++

void freeList()
{

    itrat = listNode.begin();

    while (itrat != listNode.end()) {
	subItrat = (*itrat)->subTaskListItrat->begin();
	while (subItrat != (*itrat)->subTaskListItrat->end()) {

	    free(*subItrat);
	    subItrat++;

	}
	(*itrat)->subTaskListItrat->clear();



	free(*itrat);

	OutputDebugStringW(L"n");
	OutputDebugStringW(L"Window Destroyed");


	itrat++;
    }
    subTaskList.clear();
    listNode.clear();

    i = desktopList.begin();
    while (i != desktopList.end()) {


	free(*i);



	i++;
    }
    desktopList.clear();



}

//++++++++++++++++++++ Call to add running Application to LIST  ++++++++++++++++++++++++++++++++++++++++++++++++++++++

void addList(LPARAM hwnd, HDESK desktophWnd)
{

    TCHAR szCls[0x1000] = { 0 };
    TCHAR szText[0x1000] = { 0 };
    TCHAR temp[0x80] = { 0 };
    TCHAR buf[0x100] = { 0 };
    DWORD pid = 0;
    WORD icon = 0;
    DWORD tid;

    std::list < struct taskData *>newList;

    GetWindowText((HWND) hwnd, szText, sizeof(szText));
    GetClassName((HWND) hwnd, szCls, sizeof(szCls));
    TCHAR filename[1000] = { NULL };
    tid = GetWindowThreadProcessId((HWND) hwnd, &pid);
    HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    GetModuleFileNameEx(proc, NULL, filename, sizeof(filename));
    HINSTANCE Wininstance = (HINSTANCE) GetWindowLongPtr((HWND) hwnd, GWLP_HINSTANCE);

    HICON hIcon = ExtractAssociatedIcon(Wininstance, filename, &icon);

    itrat = listNode.begin();

    while (itrat != listNode.end()) {
	if (!wcscmp((*itrat)->path, filename)) {
	    struct taskData *newSubNode = (struct taskData *) malloc(sizeof(struct taskData));
	    newSubNode->desktophWnd = desktophWnd;
	    newSubNode->WinHandle = hwnd;
	    wcscpy(newSubNode->path, filename);
	    newSubNode->hIcon = hIcon;
	    (*itrat)->subTaskListItrat->push_back(newSubNode);



	    return;
	}
	itrat++;
    }
    struct taskData *newNode = (struct taskData *) malloc(sizeof(struct taskData));
    struct taskData *newSubNode = (struct taskData *) malloc(sizeof(struct taskData));

    newSubNode->desktophWnd = desktophWnd;
    newSubNode->WinHandle = hwnd;
    wcscpy(newSubNode->path, filename);
    newSubNode->hIcon = hIcon;
    newList.push_back(newSubNode);
    subTaskList.push_front(newList);

    subTaskListItrat = subTaskList.begin();

    newNode->desktophWnd = desktophWnd;
    newNode->WinHandle = hwnd;
    wcscpy(newNode->path, filename);
    newNode->hIcon = hIcon;
    newNode->subTaskListItrat = subTaskListItrat;



    listNode.push_front(newNode);
    CloseHandle(proc);

    if (wcsstr(filename, TEXT("iexplore.exe"))) {
	EnumChildWindows((HWND) hwnd, EnumChildProc, hwnd);
    }





}

//++++++++++++++++++++ Main Starting Procedure first called after Registrating Window ++++++++++++++++++++++++++++++++++++++++++++++++++++++

void MySystemProc(HWND hWnd)
{

//      HICON hIcon;
    HWINSTA winStationhWnd;
    winStationhWnd = GetProcessWindowStation();	//Get Current Process Window Handle
    EnumDesktops(winStationhWnd, EnumDesktopProc, NULL);
    i = desktopList.begin();
    while (i != desktopList.end()) {
	if (wcscmp(*i, TEXT("Winlogon")) && wcscmp(*i, TEXT("Disconnect"))) {
	    HDESK desktopHwnd = OpenDesktop(*i, 0, FALSE, GENERIC_ALL);
	    EnumDesktopWindows(desktopHwnd, EnumWindowsProc, LPARAM(desktopHwnd));


	}
	i++;
    }


    BuildToolbar(1);
    CreateSimpleToolbar();


}

//++++++++++++++++++++ Callback Function to Enumerate Open Desktops  ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CALLBACK EnumDesktopProc(LPTSTR lpszDesktop, LPARAM lParam)
{
    TCHAR *szBuff;


    if (!checkList(lpszDesktop)) {
	szBuff = (TCHAR *) malloc(100);
	_stprintf(szBuff, _T("%s"), lpszDesktop);



	desktopList.push_front(szBuff);
    }

    OutputDebugString(L"Desktop  Enumrated");
    return 1;

}

//++++++++++++++++++++ Check in List for pre-existence of Desktop ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL checkList(LPTSTR desktopName)
{
    i = desktopList.begin();
    while (i != desktopList.end()) {

	if (!wcscmp(*i, desktopName))
	    return 1;
	i++;
    }
    return 0;

}

//++++++++++++++++++++ Create And Register Taskbar ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL BuildToolbar(int flag)
{
	 int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);

    hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_TRANSPARENT, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);

    if (hWndToolbar == NULL)
	return 1;

    g_hImageList = ImageList_Create(24, 24,	// Dimensions of individual bitmaps.
				    ILC_COLOR32 | ILC_MASK,	// Ensures transparent background.
				    0, 0);
    //.......CHILD CONTROL EDIT BOX.....................


    if (flag) {

	hWndEdit = CreateWindowEx(0, L"Edit", NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_LEFT | ES_AUTOVSCROLL | ES_MULTILINE, 1125, 0, 150, 40, hWnd, NULL, hInst, NULL);
	
	// ShowWindow(hWndEdit,TRUE);
	// SetWindowPos(hWndEdit,HWND_TOP,1125,0,150,40,SWP_NOOWNERZORDER);
	 BringWindowToTop(hWndEdit);
	// SetWindowPos(hWndToolbar,HWND_BOTTOM,0,0,0,0,SWP_NOOWNERZORDER);


	if (!hWndEdit) {
	    DestroyWindow(hWndToolbar);
	    ImageList_Destroy(g_hImageList);

	    return NULL;
	}
    }




}

//++++++++++++++++++++ Search in Taskbar ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL Search(HWND editHwnd)
{
    TCHAR text[100] = { NULL };
    const int ImageListID = 1;
    int length = Edit_GetText(editHwnd, text, sizeof(text));
    TBBUTTON tbButton, tbb, spacer;
    TBBUTTONINFO tbBtnInfo;
    DWORD buttonStyle, command;

    //OutputDebugString(text);
    if (length == 0) {

	ImageList_Remove(g_hImageList, -1);
	ImageList_Destroy(g_hImageList);

	DestroyWindow(hWndToolbar);
	BuildToolbar(0);
	CreateSimpleToolbar();
	return 1;
    }

    ImageList_Remove(g_hImageList, -1);
    ImageList_Destroy(g_hImageList);

    DestroyWindow(hWndToolbar);
    BuildToolbar(0);
    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM) ImageListID, (LPARAM) g_hImageList);
    itrat = listNode.begin();


    while (itrat != listNode.end()) {

	TCHAR szText[0x1000];
	GetWindowText((HWND) ((*itrat)->WinHandle), szText, sizeof(szText));
	int i = 0;
	while (szText[i]) {
	    szText[i] = tolower(szText[i]);
	    i++;
	}
	i = 0;
	while (text[i]) {
	    text[i] = tolower(text[i]);
	    i++;
	}

	if (wcsstr(szText, text)) {


	    int a = ImageList_AddIcon(g_hImageList, (*itrat)->hIcon);
	    if ((*itrat)->subTaskListItrat->size() == 1) {
		buttonStyle = TBSTYLE_BUTTON;
		command = ID_PORT_BUTTON;

	    } else {
		buttonStyle = TBSTYLE_DROPDOWN;
		command = ID_PORT_BUTTON;

	    }


	    tbButton.fsState = TBSTATE_ENABLED;
	    tbButton.fsStyle = buttonStyle;
	    tbButton.iBitmap = a;
	    tbButton.idCommand = command;
	    tbButton.iString = SendMessage(hWndToolbar, TB_ADDSTRING, NULL, (LPARAM) (szText));
	    tbButton.dwData = 0;

	    tbb.iBitmap = 0;
	    tbb.fsState = TBSTATE_ENABLED;
	    tbb.fsStyle = BTNS_SEP;
	    tbb.idCommand = 0;	// Sent in WM_COMMAND when pressed                                              // Sent in WM_COMMAND when pressed
	    tbb.iString = 0;


	    spacer.iBitmap = 0;
	    spacer.fsState = TBSTATE_ENABLED;
	    spacer.fsStyle = BTNS_SEP;
	    spacer.idCommand = 0;	// Sent in WM_COMMAND when pressed                                              // Sent in WM_COMMAND when pressed
	    spacer.iString = 0;



	    tbBtnInfo.cbSize = sizeof(TBBUTTONINFO);
	    tbBtnInfo.cchText = sizeof(szText);
	    tbBtnInfo.cx = 24;
	    tbBtnInfo.dwMask = TBIF_LPARAM;
	    tbBtnInfo.fsState = TBSTATE_ENABLED;
	    tbBtnInfo.fsStyle = buttonStyle;
	    tbBtnInfo.idCommand = command;
	    tbBtnInfo.iImage = a;
	    tbBtnInfo.lParam = ((*itrat)->WinHandle);
	    tbBtnInfo.pszText = szText;




	    SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 15);



	    SendMessage(hWndToolbar, TB_SETPADDING, 0, MAKELONG(15, 15));
	    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(tbButton), 0);

	    SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	    SendMessage(hWndToolbar, TB_INSERTBUTTON, (WPARAM) 1, (LPARAM) & tbb);
	    SendMessage(hWndToolbar, TB_INSERTBUTTON, (WPARAM) 1, (LPARAM) & tbButton);

	    if (!SendMessage(hWndToolbar, TB_SETBUTTONINFO, (WPARAM) ID_PORT_BUTTON, (LPARAM) & tbBtnInfo))
		OutputDebugString(L"ERRORRRRRRRRR");


	    SendMessage(hWndToolbar, TB_SETBUTTONSIZE, NULL, MAKELONG(40, 36));





	}


	itrat++;
    }
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    ShowWindow(hWndToolbar, TRUE);

    return 1;
}

//++++++++++++++++++++ To Enumerate Internet Explorer Tabs ++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CALLBACK EnumChildProc(HWND childhWnd, LPARAM lParam)
{

    HICON hIcon;
    WORD icon = 0;
    TCHAR szCls[0x1000] = { 0 };
    TCHAR szText[0x1000] = { 0 };
    TCHAR temp[0x80] = { 0 };
    TCHAR buf[0x100] = { 0 };
    TCHAR stringTitle[1000] = { NULL };
    int length;
    BOOL flag = 1;
    HDESK desktophWnd = GetThreadDesktop(GetCurrentThreadId());
    DWORD pid = 0;
    TCHAR tab[] = TEXT("TabWindowClass");
    std::list < struct taskData *>::iterator itr;
    std::list < struct taskData *>newList;
    length = GetWindowText(childhWnd, stringTitle, sizeof(stringTitle));

    GetWindowText(childhWnd, szText, sizeof(szText));
    GetClassName(childhWnd, szCls, sizeof(szCls));
    TCHAR filename[1000] = { NULL };
    GetWindowThreadProcessId((HWND) lParam, &pid);
    HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    GetModuleFileNameEx(proc, NULL, (LPWSTR) filename, sizeof(filename));

    HINSTANCE Wininstance = (HINSTANCE) GetWindowLongPtr(childhWnd, GWLP_HINSTANCE);

    hIcon = ExtractAssociatedIcon(Wininstance, filename, &icon);


    OutputDebugString(L"Ïn AddtoList");

    if (!lstrcmp(szCls, tab)) {
	OutputDebugString(L"TAB OPPEN n");
	itr = listNode.begin();
	while (itr != listNode.end()) {

	    if (!wcscmp((*itr)->path, filename)) {



		struct taskData *newNode = (struct taskData *) malloc(sizeof(struct taskData));
		newNode->desktophWnd = desktophWnd;
		newNode->WinHandle = (LPARAM) childhWnd;
		wcscpy(newNode->path, filename);
		newNode->hIcon = hIcon;
		flag = 1;
		(*itr)->subTaskListItrat->push_back(newNode);





	    }
	    itr++;
	}






    }

    CloseHandle(proc);

    return 1;


}
