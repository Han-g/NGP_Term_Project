// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "NetworkGame_Proj.h"
#include "Common.h"
#include "Global.h"

#include "Interact.h"

#define MAX_LOADSTRING 100
#define SERVERPORT 9000
#define BUFSIZE    50000

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HANDLE hMsgThread;                              // 통신 스레드
DWORD dwMsgThreadID;                            // 통신 스레드 ID

ServerData Server_bufArray[4];

// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

typedef struct box {
    int left, top, right, bottom;
} box;

// 오브젝트 통제 변수
//EventHandle g_event = nullptr;

DWORD g_Time = 0;
DWORD g_startTime = 0;
DWORD g_prevTime = 0;

Interact* g_Interact;

bool Exitcode = false;
int clientNum = 0;

char* SERVERIP = (char*)"127.0.0.1";
SOCKET sock; // 소켓
ServerData buf; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    if (objInfoSize != 0 && data->object_info.size() != 0) {
        data->object_info.resize(objInfoSize);
        std::memcpy(data->object_info.data(), buf, objInfoSize * sizeof(obj_info));
    }
}

void CommuicateThread() {
    hMsgThread = CreateThread(NULL, 0, ClientMain, NULL, 0, &dwMsgThreadID);
    if (hMsgThread == NULL) {
        delete hMsgThread;
    }
}

void InitObjects(int index) {
    obj_info Player1_Index;
    obj_info Player1_bubble[4];
    obj_info Background[15][15];

    switch (index) {
    case 0:
        Player1_Index = {
            0, 0,
            0, 0,
            Char_Idle, 0, { 0, 0 }
        };
        break;
    case 1:
        Player1_Index = {
            15, 0,
            0, 0,
            Char_Idle, 0, { 0, 0 }
        };
        break;
    case 2:
        Player1_Index = {
            0, 15,
            0, 0,
            Char_Idle, 0, { 0, 0 }
        };
        break;
    case 3:
        Player1_Index = {
            15, 15,
            0, 0,
            Char_Idle, 0, { 0, 0 }
        };
        break;
    default:
        break;
    }

    Server_bufArray[clientNum].buf.object_info.push_back(Player1_Index);

    for (int i = 0; i < 4; i++)
    {
        Player1_bubble[i] = {
            Player1_Index.posX, Player1_Index.posY,
            0, 0,
            Non_Bubble, 0, { 0, 0 }
        };
        Server_bufArray[clientNum].buf.object_info.push_back(Player1_bubble[i]);
    }


    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            Background[i][j] = {
                0, 0,
                0, 0,
                Non_Obj, 0, { 0, 0 }
            };
            Server_bufArray[clientNum].buf.object_info.push_back(Background[i][j]);
        }
    }
}

void RenderBackground(PAINTSTRUCT ps, HDC hdc) {
    static HBITMAP hBit;
    hBit = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP17));
    HDC memdc = CreateCompatibleDC(hdc);
    SelectObject(memdc, hBit);

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            BitBlt(hdc, 0 + (i * 52), 0 + (j * 52), 52, 52, memdc, 0, 0, SRCCOPY);
        }
    }
    DeleteDC(memdc);
}

void RenderChar(PAINTSTRUCT ps, HDC hdc) {
    static HBITMAP hBit;
    HDC memdc = CreateCompatibleDC(hdc);

    for (int i = 0; i < 4; i++) {
        for (const obj_info& info : Server_bufArray[i].buf.object_info) {
            if (checkStatus(info.type)) {
                switch (info.type)
                {
                    // tiles -> 52, 52 | char -> 64, 76 | bubble 56, 54
                // character
                case Char_Idle:
                case Char_Right:
                case Char_Left:
                case Char_Up:
                case Char_Down:
                    hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
                    SelectObject(memdc, hBit);
                    BitBlt(hdc, info.posX * 52, info.posY * 52, 64, 76, memdc, 0, 0, SRCCOPY);
                    break;

                    // bubble
                case Bubble_Idle:
                case Bubble_bomb:
                    //case Non_Bubble:
                    hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
                    SelectObject(memdc, hBit);
                    BitBlt(hdc, info.posX * 52, info.posY * 52, 56, 54, memdc, 0, 0, SRCCOPY);
                    break;

                    // background
                case BackGround:
                case Bg_tile1:
                case Bg_tile2:
                case Bg_tile3:
                case Bg_tile4:
                    hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP16));
                    SelectObject(memdc, hBit);
                    BitBlt(hdc, info.posX * 52, info.posY * 52, 52, 52, memdc, 0, 0, SRCCOPY);
                    break;
                default:
                    break;
                }
            }
        }
    }
    DeleteDC(memdc);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;
    DWORD exitCode;

    // 기본 메시지 루프입니다:
    while (!Exitcode && GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetExitCodeThread(hMsgThread, &exitCode) && exitCode != STILL_ACTIVE)
        {
            Exitcode = true;
        }
    }

    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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

    int FrameX = GetSystemMetrics(SM_CXSCREEN), FrameY = GetSystemMetrics(SM_CYSCREEN),
        Caption = GetSystemMetrics(SM_CYCAPTION);

    HWND hWnd = CreateWindowW(
        szWindowClass,               // 윈도우 클래스 이름
        szTitle,                     // 윈도우 타이틀 이름
        WS_OVERLAPPEDWINDOW,         // 윈도우 스타일
        (FrameX - window_size_w) / 2,
        (FrameY - window_size_d) / 2,// 윈도우 생성 위치
        window_size_w, window_size_d,
        nullptr,                     // 부모 윈도우 핸들 HWND
        nullptr,                     // 메뉴 핸들 HMENU
        hInstance,                   // 응용 프로그램 인스턴스 HINSTANCE
        nullptr                      // 생성 윈도우 정보 LPVOID
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
    switch (message)
    {
    case WM_CREATE:
        g_Interact = new Interact();
        InitObjects(clientNum);

        hReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        CommuicateThread();
        break;
    case WM_KEYDOWN:
        if (g_Interact->checkKeyInput(wParam)) {
            Server_bufArray[clientNum].buf.wParam = wParam;
        }
        break;
    case WM_KEYUP:
        g_Interact->resetKeyInput();
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
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RenderBackground(ps, hdc);
        RenderChar(ps, hdc);
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

DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) { err_quit("socket()"); }

    // connect
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

    int clientIndex = 0;
    bool recvCheck = FALSE;
    retval = recv(sock, (char*)&clientIndex, sizeof(int), 0); // 0,1,2,3
    InitObjects(clientIndex);

    while (1) {
        fd_set rset, wset;
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        if (!recvCheck) {
            FD_SET(sock, &wset);
        }
        else {
            FD_SET(sock, &rset);
        }

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 16; // 1ms

        int nready = select(0, &rset, &wset, NULL, &timeout);
        if (nready == SOCKET_ERROR) {
            err_display("select()");
            break;
        }

        else {
            char buffer[BUFSIZE];

            if (FD_ISSET(sock, &wset)) {
                if (Server_bufArray[clientIndex].buf.object_info.size() != 0) {
                    Serialize(&buf.buf, buffer, BUFSIZE);
                    retval = send(sock, buffer, BUFSIZE, 0);
                    if (retval == SOCKET_ERROR || retval == 0) {
                        break;
                    }
                }
                else {
                    printf("Buffer is empty!\n");
                }
                recvCheck = FALSE;
            }

            else if (FD_ISSET(sock, &rset)) {
                int nTotalSocket = 0;
                retval = recv(sock, (char*)&nTotalSocket, sizeof(int), MSG_WAITALL);
                if (retval == SOCKET_ERROR || retval == 0) {
                    err_display("recv()");
                    break;
                }

                retval = recv(sock, buffer, sizeof(char) * BUFSIZE, 0);
                if (retval == SOCKET_ERROR || retval == 0) {
                    err_display("recv()");
                    break;
                }
                else {
                    // 받은 데이터 처리
                    for (const char& c : buffer) {
                        if (c != 0) {
                            DeSerialize(&buf.buf, buffer, BUFSIZE);
                            if (Server_bufArray[clientIndex].buf.object_info.size() != 0
                                && Server_bufArray[clientIndex].buf.object_info.capacity() != 0) {
                                Server_bufArray[clientIndex].buf.object_info.clear();
                                size_t objSize = Server_bufArray[clientIndex].buf.object_info.capacity() * sizeof(obj_info);
                                Server_bufArray[clientIndex].buf.object_info.reserve(objSize);
                                Server_bufArray[clientIndex].buf.object_info = buf.buf.object_info;
                            }
                            break;
                        }
                    }
                }

                recvCheck = TRUE;
            }
        }
    } // while

    closesocket(sock);
    WSACleanup();

    Exitcode = TRUE;
    return 0;
}





// 클라이언트 정보 전달
//if (buf.buf.object_info.size() != 0) {
//    Serialize(&buf.buf, buffer, BUFSIZE);
//    retval = send(sock, buffer, BUFSIZE, 0);
//    if (retval == SOCKET_ERROR || retval == 0) {
//        break;
//    }
//}
//else {
//    printf("Buffer is empty!\n");
//}

//int nTotalSocket = 0;
//retval = recv(sock, (char*)&nTotalSocket, sizeof(int), MSG_WAITALL);
//if (retval == SOCKET_ERROR || retval == 0) {
//    err_display("recv()");
//    break;
//}

//for (int i = 0; i < nTotalSocket; i++) {
//    retval = recv(sock, buffer, sizeof(char) * BUFSIZE, MSG_WAITALL);
//    if (retval == SOCKET_ERROR || retval == 0) {
//        err_display("recv()");
//        break;
//    }
//    else {
//        for (const char& c : buffer) {
//            if (c != 0) {
//                DeSerialize(&buf.buf, buffer, BUFSIZE);
//                Server_bufArray[i].buf.object_info.clear();
//                size_t objSize = Server_bufArray[i].buf.object_info.capacity() * sizeof(obj_info);
//                Server_bufArray[i].buf.object_info.reserve(objSize);
//                Server_bufArray[i].buf.object_info = buf.buf.object_info;
//                std::cout << Server_bufArray[i].buf.object_info[225].posX << ", "
//                    << Server_bufArray[i].buf.object_info[225].posY << std::endl;
//                break;
//            } // if
//        } // for
//    } // else
//} // for