#include "Common.h"
#include "framework.h"

#include "NetworkGame_Proj.h"
#include "GameSet.h"
#include "Obj_Interaction.h"
#include "EventHandle.h"
#include "Global.h"

//#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HANDLE hMsgThread;                              // 통신 스레드
DWORD dwMsgThreadID;                            // 통신 스레드 ID

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

typedef struct box {
    int left, top, right, bottom;
} box;

// 오브젝트 통제 변수
GameSet* g_game = NULL;
//EventHandle g_event = nullptr;

DWORD g_Time = 0;
DWORD g_startTime = 0;
DWORD g_prevTime = 0;

void Serialize(Send_datatype* data, char* buf, size_t bufSize) {
    if (bufSize < sizeof(Send_datatype)) {
        return;
    }

    // Send_datatype 구조체의 멤버들을 바이트로 복사
    std::memcpy(buf, &data, sizeof(Send_datatype));
}

void DeSerialize(Send_datatype* data, char* buf, size_t bufSize) {
    if (bufSize < sizeof(Send_datatype)) {
        return;
    }

    // 바이트를 Send_datatype 구조체로 복사
    std::memcpy(&data, buf, sizeof(Send_datatype));
}

void CommuicateThread() {
    hMsgThread = CreateThread(NULL, 0, ClientMain, NULL, 0, &dwMsgThreadID);
    if (hMsgThread == NULL) {
        delete hMsgThread;
    }
}

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
    DeleteDC(memdc);
}

void RenderScene(HDC hdc) {
    g_game->DrawAll(hdc, hInst);
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
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        g_game->getTime(g_Time++);
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
    //EventHandle g_handle();
    PAINTSTRUCT ps;
    HDC hdc = GetDC(hWnd);
    
    switch (message)
    {
    case WM_CREATE:
        hdc = BeginPaint(hWnd, &ps);
        g_game = new GameSet(hdc);
        g_Interaction = new Obj_Interaction;
        CommuicateThread();
        break;
    case WM_KEYDOWN:
        g_game->KeyInput(g_Interaction, wParam);
        CommuicateThread();
        break;
    case WM_KEYUP:
        InvalidateRect(hWnd, NULL, TRUE);
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
            RenderScene(hdc);
            EndPaint(hWnd, &ps);
            ++g_Time;
        }
        break;
    case WM_DESTROY:
        TerminateThread(hMsgThread, 0);
        WaitForSingleObject(hMsgThread, INFINITE);
        CloseHandle(hMsgThread);

        delete g_Interaction;
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


// 대화상자 프로시저
INT_PTR CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...);
// 소켓 함수 오류 출력
void DisplayError(const char* msg);


char* SERVERIP = (char*)"127.0.0.1";
SOCKET sock; // 소켓
Send_datatype buf; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hEdit1, hEdit2; // 에디트 컨트롤


// 대화상자 프로시저
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_INITDIALOG:
        //hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
        //hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
        hSendButton = GetDlgItem(hDlg, IDOK);
        SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
            WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 대기
            //GetDlgItemTextA(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
            SetEvent(hWriteEvent); // 쓰기 완료 알림
            SetFocus(hEdit1); // 키보드 포커스 전환
            SendMessage(hEdit1, EM_SETSEL, 0, -1); // 텍스트 전체 선택
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL); // 대화상자 닫기
            closesocket(sock); // 소켓 닫기
            return TRUE;
        }
        return FALSE;
    }
    return FALSE;
}

// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    char cbuf[BUFSIZE * 2];
    vsprintf(cbuf, fmt, arg);
    va_end(arg);

    int nLength = GetWindowTextLength(hEdit2);
    SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
    SendMessageA(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
}

// 소켓 함수 오류 출력
void DisplayError(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (char*)&lpMsgBuf, 0, NULL);
    DisplayText("[%s] %s\r\n", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // connect()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");

    // 서버와 데이터 통신
    while (1) {
        WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 대기

        // 문자열 길이가 0이면 보내지 않음
        //if (strlen(buf) == 0) {
        //    EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        //    SetEvent(hReadEvent); // 읽기 완료 알림
        //    continue;
        //}

        // 데이터 보내기
        g_game->getObjINFO(&buf);
        char buffer[BUFSIZE];
        Serialize(&buf, buffer, sizeof(Send_datatype));

        retval = send(sock, buffer, sizeof(Send_datatype), 0);
        if (retval == SOCKET_ERROR) {
            DisplayError("send()");
            break;
        }
        DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다.\r\n", retval);

        // 데이터 받기
        retval = recv(sock, buffer, retval, MSG_WAITALL);
        DeSerialize(&buf, buffer, sizeof(Send_datatype));

        if (retval == SOCKET_ERROR) {
            DisplayError("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 받은 데이터 출력
        //buf[retval] = '\0';
        DisplayText("[TCP 클라이언트] %d바이트를 받았습니다.\r\n", retval);
        DisplayText("[받은 데이터] %s\r\n", buf);

        EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        SetEvent(hReadEvent); // 읽기 완료 알림
    }

    return 0;
}