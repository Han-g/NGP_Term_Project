#include "Common.h"
#include "framework.h"

#include "NetworkGame_Proj.h"
#include "GameSet.h"
#include "Obj_Interaction.h"
#include "EventHandle.h"
#include "Global.h"

//#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    50000

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HANDLE hMsgThread;                              // 통신 스레드
DWORD dwMsgThreadID;                            // 통신 스레드 ID

Send_datatype Server_bufArray[4];

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

bool Exitcode = false;

char* SERVERIP = (char*)"127.0.0.1";
SOCKET sock; // 소켓
Send_datatype buf; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hEdit1, hEdit2; // 에디트 컨트롤

void Serialize(Send_datatype* data, char* buf, size_t bufSize) {
    // 데이터 크기 확인
    size_t dataSize = sizeof(int) + sizeof(double) + data->object_info.size() * sizeof(obj_info);

    // 버퍼 크기 확인
    if (bufSize < dataSize) {
        std::cerr << "Buffer size is too small for serialization!" << std::endl;
        return;
    }

    // 버퍼 초기화
    memset(buf, 0, dataSize);

    // 데이터 복사
    std::memcpy(buf, &data->wParam, sizeof(int));
    buf += sizeof(int);

    std::memcpy(buf, &data->GameTime, sizeof(double));
    buf += sizeof(double);

    std::memcpy(buf, data->object_info.data(), data->object_info.size() * sizeof(obj_info));
}

void DeSerialize(Send_datatype* data, char* buf, size_t bufSize) {
    if (bufSize < sizeof(int) + sizeof(double)) {
        std::cerr << "Buffer size is too small for deserialization!" << std::endl;
        return;
    }

    // 버퍼 초기화
    data->object_info.clear();
    data->GameTime = 0.0f;
    data->wParam = 0;

    // 데이터 복사
    std::memcpy(&data->wParam, buf, sizeof(int));
    buf += sizeof(int);

    std::memcpy(&data->GameTime, buf, sizeof(double));
    buf += sizeof(double);

    // obj_info 역직렬화
    size_t objInfoSize = (bufSize - sizeof(int) - sizeof(double)) / sizeof(obj_info);
    data->object_info.resize(objInfoSize);
    std::memcpy(data->object_info.data(), buf, objInfoSize * sizeof(obj_info));
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
    while (!Exitcode &&GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        g_game->getTime(g_Time++);

        DWORD exitCode;
        if (GetExitCodeThread(hMsgThread, &exitCode) && exitCode != STILL_ACTIVE)
        {
            Exitcode = true;
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
    //EventHandle g_handle();
    PAINTSTRUCT ps;
    HDC hdc = GetDC(hWnd);
    
    switch (message)
    {
    case WM_CREATE:
        hdc = BeginPaint(hWnd, &ps);
        g_game = new GameSet(hdc);
        g_Interaction = new Obj_Interaction;

        hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        CommuicateThread();
        //Update();
        break;
    case WM_KEYDOWN:
        g_game->KeyInput(g_Interaction, wParam);
        //CommuicateThread();
        SetEvent(hWriteEvent);
        break;
    case WM_KEYUP:
        InvalidateRect(hWnd, NULL, TRUE);
        ResetEvent(hWriteEvent);
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

            //WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 대기
            ResetEvent(hReadEvent); // 처리 후 이벤트 재설정
        }
        break;
    case WM_DESTROY:
        TerminateThread(hMsgThread, 0);
        WaitForSingleObject(hMsgThread, INFINITE);
        CloseHandle(hMsgThread);

        if (g_Interaction != NULL) { delete g_Interaction; }
        
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
    if (retval == SOCKET_ERROR) {
        err_quit("connect()");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 게임 프레임
    const int framesPerSec = 60;
    const int frameDelay = 1000 / framesPerSec;

    // 서버와 데이터 통신
    while (1) {
        DWORD startTime = GetTickCount();
        //WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 대기

        // 문자열 길이가 0이면 보내지 않음
        //if (strlen(buf) == 0) {
        //    EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        //    SetEvent(hReadEvent); // 읽기 완료 알림
        //    continue;
        //}
        
        // 데이터 보내기
        g_game->getObjINFO(&buf);
        buf.wParam = g_game->Key_return();
        char buffer[BUFSIZE];
        memset(buffer, NULL, sizeof(char) * BUFSIZE);

        // 클라이언트 정보 전달
        Serialize(&buf, buffer, sizeof(char) * BUFSIZE);
        retval = send(sock, buffer, sizeof(char) * BUFSIZE, 0);
        if (retval == SOCKET_ERROR || retval == 0) {
            break;
        }
        //DisplayText("[TCP 클라이언트] %d바이트를 보냈습니다.\r\n", retval);

        // 데이터 받기
        
        size_t receivedData = 0;
        size_t dataSize = 0;

        // 수정된 클라이언트 정보 받아오기
        retval = recv(sock, buffer, sizeof(char)*BUFSIZE, MSG_WAITALL);
        DeSerialize(&buf, buffer, sizeof(Send_datatype));
        if (buf.object_info.size() > 0) { g_game->updateObjINFO(buf); }
        
        // 전체 클라이언트 정보 받아오기
        for (int i = 0; i < 4; i++) {
            retval = recv(sock, buffer, sizeof(char) * BUFSIZE, MSG_WAITALL);
            DeSerialize(&buf, buffer, BUFSIZE);
            Server_bufArray[i] = buf;
        }

        EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        SetEvent(hReadEvent); // 읽기 완료 알림

        DWORD elapsedTime = GetTickCount() - startTime;
        int remainingTime = frameDelay - elapsedTime;

        if (remainingTime > 0) {
            Sleep(remainingTime);
        }
    }

    closesocket(sock);
    WSACleanup();
    Exitcode = TRUE;

    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);
    return 0;
}