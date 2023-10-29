#include "Common.h"
#include "framework.h"

#include "NetworkGame_Proj.h"
#include "GameSet.h"
#include "Obj_Interaction.h"
#include "EventHandle.h"
#include "Global.h"

#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

typedef struct box {
    int left, top, right, bottom;
} box;

// 오브젝트 통제 변수
GameSet* g_game = NULL;
//EventHandle g_event = NULL;

DWORD g_startTime = 0;
DWORD g_prevTime = 0;

void RenderBackground(PAINTSTRUCT ps, HDC hdc) {
    static HBITMAP hBit;
    hBit = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
    HDC memdc = CreateCompatibleDC(hdc);
    SelectObject(memdc, hBit);

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            BitBlt(hdc, 0 + (i * 52), 0 + (j * 52), 52, 52, memdc, 0, 0, SRCCOPY);
        }
    }
    //DeleteDC(memdc);
}

void RenderScene() {
    g_game->DrawAll(hInst);
}

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK    Dlalog_Proc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(  _In_ HINSTANCE hInstance,
                        _In_opt_ HINSTANCE hPrevInstance,
                        _In_ LPWSTR    lpCmdLine,
                        _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NETWORKGAMEPROJ, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NETWORKGAMEPROJ));
    
    MSG msg;

    int FrameX = GetSystemMetrics(SM_CXFRAME), FrameY = GetSystemMetrics(SM_CYFRAME), 
        Caption = GetSystemMetrics(SM_CYCAPTION);


    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            //RenderScene();
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW WndClass;

    WndClass.cbSize = sizeof(WNDCLASSEX);

    WndClass.style          = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc    = WndProc;
    WndClass.cbClsExtra     = 0;
    WndClass.cbWndExtra     = 0;
    WndClass.hInstance      = hInstance;
    WndClass.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NETWORKGAMEPROJ));
    WndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    WndClass.lpszMenuName   = MAKEINTRESOURCEW(IDC_NETWORKGAMEPROJ);
    WndClass.lpszClassName  = szWindowClass;
    WndClass.hIconSm        = LoadIcon(WndClass.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&WndClass);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(
       szWindowClass,           // 윈도우 클래스 이름
       szTitle,                 // 윈도우 타이틀 이름
       WS_OVERLAPPEDWINDOW,     // 윈도우 스타일
       100, 100,                // 윈도우 생성 위치
       window_size_w, window_size_d, 
       nullptr,                 // 부모 윈도우 핸들 HWND
       nullptr,                 // 메뉴 핸들 HMENU
       hInstance,               // 응용 프로그램 인스턴스 HINSTANCE
       nullptr                  // 생성 윈도우 정보 LPVOID
   );

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Obj_Interaction* g_Interaction = NULL;
    //EventHandle g_handle(wParam);
    PAINTSTRUCT ps;
    HDC hdc = GetDC(hWnd);
    
    switch (message)
    {
    case WM_CREATE:
        hdc = BeginPaint(hWnd, &ps);
        g_game = new GameSet(hdc);
        break;
    case WM_KEYDOWN:
        //g_handle.checkEvent();
        RenderScene();
        break;
    case WM_KEYUP:
        //g_handle.ResetEvent();
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            RenderBackground(ps, hdc);
            RenderScene();
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
